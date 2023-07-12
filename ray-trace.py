import numpy as np
import math
import sys
from PIL import Image


# READ AND LOAD FROM FILE

def parse_camera_data(lines):
    camera_data = {}
    camera_data['position'] = [float(x) for x in lines[0].split()]
    camera_data['target'] = [float(x) for x in lines[1].split()]
    camera_data['normal'] = [float(x) for x in lines[2].split()]
    camera_data['aperture'] = float(lines[3])
    return camera_data


def parse_light_data(lines):
    num_lights = int(lines[0])
    light_data = []
    for i in range(1, num_lights + 1):
        light = {}
        light['position'] = [float(x) for x in lines[i].split()[:3]]
        light['color'] = [float(x) for x in lines[i].split()[3:6]]
        light['attenuation'] = [float(x) for x in lines[i].split()[6:]]
        light_data.append(light)
    return light_data

def parse_pigment_data(lines):
    num_pigments = int(lines[0])
    pigment_data = []
    line_index = 1
    for i in range(num_pigments):
        
        pigment = {}
        line = lines[line_index].split()
        
        pigment['type'] = line[0]
        
        if pigment['type'] == 'solid':
            pigment['color'] = [float(x) for x in line[1:4]]
            
        elif pigment['type'] == 'checker':
            pigment['color1'] = [float(x) for x in line[1:4]]
            pigment['color2'] = [float(x) for x in line[4:7]]
            pigment['cube_length'] = float(line[7])
            
        elif pigment['type'] == 'texmap':
            pigment['texture_file'] = line[1]
            pigment['P0'] = [float(x) for x in lines[line_index + 1].split()]
            pigment['P1'] = [float(x) for x in lines[line_index + 2].split()]
            line_index += 2
            
        pigment_data.append(pigment)
        
        line_index += 1
    return pigment_data, line_index


def parse_finish_data(lines):
    num_finishes = int(lines[0])
    finish_data = []
    for i in range(1, num_finishes + 1):
        finish = {}
        line = lines[i].split()
        finish['ka'] = float(line[0])
        finish['kd'] = float(line[1])
        finish['ks'] = float(line[2])
        finish['alpha'] = float(line[3])
        finish['kr'] = float(line[4])
        finish['kt'] = float(line[5])
        finish['ior'] = float(line[6])
        finish_data.append(finish)
    return finish_data


def parse_object_data(lines):
    num_objects = int(lines[0])
    object_data = []
    for i in range(1, num_objects + 1):
        obj = {}
        line = lines[i].split()
        obj['pigment_ref'] = int(line[0])
        obj['surface_ref'] = int(line[1])
        obj_type = line[2]
        if obj_type == 'sphere':
            obj['center'] = [float(x) for x in line[3:6]]
            obj['radius'] = float(line[6])
            obj['type'] = 'sphere'
        elif obj_type == 'polyhedron':
            num_faces = int(line[3])
            obj['faces'] = []
            for j in range(num_faces):
                face_coeffs = [float(x) for x in lines[i + 1 + j].split()]
                obj['faces'].append(face_coeffs)
            obj['type'] = 'polyhedron'
        object_data.append(obj)
    return object_data


def parse_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    camera_data = parse_camera_data(lines[:4])
    light_data = parse_light_data(lines[4:])
    pigment_data, index = parse_pigment_data(lines[4 + len(light_data) + 1:])
    finish_data = parse_finish_data(lines[4 + len(light_data) + 1 + index:])
    object_data = parse_object_data(lines[4 + len(light_data) + 1 + index + len(finish_data) + 1:])

    return {
        'camera': camera_data,
        'lights': light_data,
        'pigments': pigment_data,
        'finishes': finish_data,
        'objects': object_data
    }


#CLASSES 

class Sphere:
    def __init__(self, center, radius, pigment_id, surface_id, color):
        self.center = np.array(center)
        self.radius = radius
        self.color = np.array(color)
        self.pigment_id = pigment_id
        self.surface_id = surface_id

    def ray_intersection(self, ray_origin, ray_direction):
        oc = ray_origin - self.center
        a = np.dot(ray_direction, ray_direction)
        b = 2 * np.dot(oc, ray_direction)
        c = np.dot(oc, oc) - self.radius ** 2

        discriminant = b ** 2 - 4 * a * c

        if discriminant < 0:
            return np.inf

        t1 = (-b + np.sqrt(discriminant)) / (2 * a)
        t2 = (-b - np.sqrt(discriminant)) / (2 * a)

        if t1 >= 0 and t2 >= 0:
            return min(t1, t2)
        elif t1 >= 0:
            return t1
        elif t2 >= 0:
            return t2
        else:
            return np.inf

    def get_pigment_color(self, hit_point, textures):
        if self.pigment_id == 2:
            return self.color
        
        # TODO: this should be checker
        if self.pigment_id == 1:
            return self.color
        
        if self.pigment_id == 0:
            texture = textures[self.pigment_id]
            u = np.arctan2(hit_point[2], hit_point[0]) / (2 * np.pi) + 0.5
            v = np.arccos(hit_point[1] / self.radius) / np.pi
            return texture.get_color(u, v)


class Camera:
    def __init__(self, position, target, orientation, aperture):
        self.position = np.array(position)
        self.target = np.array(target)
        self.orientation = np.array(orientation)
        self.aperture = aperture

        self.forward = self.target - self.position
        self.forward = self.forward / np.linalg.norm(self.forward)

        right = np.cross(self.forward, self.orientation)
        self.right = right / np.linalg.norm(right)

        self.up = np.cross(self.right, self.forward)

    def generate_ray_direction(self, x, y, width, height):
        aspect_ratio = width / height
        fov_tan = np.tan(self.aperture / 2)
        normalized_x = (2 * (x + 0.5) / width - 1) * fov_tan * aspect_ratio
        normalized_y = (1 - 2 * (y + 0.5) / height) * fov_tan

        direction = self.forward + self.right * normalized_x + self.up * normalized_y
        return direction / np.linalg.norm(direction)


class Light:
    def __init__(self, position, color, attenuation):
        self.position = np.array(position)
        self.color = np.array(color)

        # coeficiente constante de atenuação, atenuação proporcional à distância da fonte da luz e coeficiente de 
        # e coeficiente de atenuação proporcional ao quadrado da distância da fonte da luz
        self.attenuation = np.array(attenuation) 

        
class Texture:
    def __init__(self, image_path):
        self.image = Image.open(image_path).convert("RGB")
        self.width = self.image.width
        self.height = self.image.height

    def get_color(self, u, v):
        x = int(u * self.width) % self.width
        y = int(v * self.height) % self.height
        return self.image.getpixel((x, y))



#RAY TRACER
def trace_ray(ray_origin, ray_direction, objects, lights, textures):
    closest_t = np.inf
    closest_object = None

    for obj in objects:
        t = obj.ray_intersection(ray_origin, ray_direction)
        if t < closest_t:
            closest_t = t
            closest_object = obj

    if closest_object is None:
        return np.array([0, 0, 0])

    hit_point = ray_origin + ray_direction * closest_t
    normal = (hit_point - closest_object.center) / closest_object.radius

    # Ambient color
    ambient_color = closest_object.get_pigment_color(hit_point, textures) * 0.1

    # Diffuse color
    diffuse_color = np.array([0, 0, 0])
    for light in lights:
        light_direction = light.position - hit_point
        light_distance = np.linalg.norm(light_direction)
        light_direction = light_direction / light_distance

        # Shadow check
        shadow_origin = hit_point + light_direction * 0.001
        shadow_t = np.inf
        for obj in objects:
            t = obj.ray_intersection(shadow_origin, light_direction)
            if 0.001 < t < light_distance:
                shadow_t = t
                break

        if shadow_t == np.inf:
            diffuse_intensity = np.maximum(np.dot(normal, light_direction), 0)
            
            diffuse_effect = closest_object.get_pigment_color(hit_point, textures) * light.color
            diffuse_effect[0] *= diffuse_intensity
            diffuse_effect[1] *= diffuse_intensity
            diffuse_effect[2] *= diffuse_intensity

    
            array = [int(diffuse_effect[0]) , int(diffuse_effect[1]), int(diffuse_effect[2])  ]
            
            diffuse_color += array

    color = ambient_color + diffuse_color
    return np.minimum(color, 255)



def main():

    # Scene configuration
    width = 800
    height = 600

#     if (len(sys.argv)) == 3:
#         width = int(sys.argv[1])
#         height = int(sys.argv[2])

    # LOAD FROM FILE
    data = parse_file('scene.txt')

    # Get Pigments
    textures = []
    solid_color = []
    for pigment in data['pigments']:

        if pigment.get('type') == 'solid':
            solid_color = pigment['color']

        elif pigment.get('type') == 'texmap':
            t = Texture(pigment['texture_file'])
            # TODO: texcoords
            textures.append(t)

        # TODO: checker
        #else:
            

    # Create objects
    objects = []
    for obj in data['objects']:

        # shpere
        if obj['type'] == 'sphere':
            if (obj['pigment_ref'] == 2): # solid
                color = solid_color
            # TODO
            else: color = solid_color
            
            s = Sphere(center=obj['center'], radius=obj['radius'], color=color, pigment_id=obj['pigment_ref'], surface_id=obj['surface_ref'])
            objects.append(s)

        # TODO: “polyhedron”
        #else:
        


    # Create lights
    lights = []
    for i in range(len(data['lights'])):
        light = Light(position=data['lights'][i]['position'], color=data['lights'][i]['color'], attenuation=data['lights'][i]['attenuation'])
        lights.append(light)


    # Create camera
    camera = Camera( position=data['camera']['position'], target=data['camera']['target'], orientation=data['camera']['normal'], aperture=math.radians(data['camera']['aperture']))

    # Create image
    image = np.zeros((height, width, 3), dtype=np.uint8)

    for y in range(height):
        for x in range(width):
            ray_direction = camera.generate_ray_direction(x, y, width, height)

            color = trace_ray(camera.position, ray_direction, objects, lights, textures)

            image[y, x] = color

    image = Image.fromarray(image)
    image.save("output.png")
    image.show()


def create_rainbow_ppm(width, height):
    image = [['0 0 0' for _ in range(width)] for _ in range(height)]

    # Rainbow colors
    colors = [
        (255, 0, 0),    # Red
        (255, 165, 0),  # Orange
        (255, 255, 0),  # Yellow
        (0, 255, 0),    # Green
        (0, 0, 255),    # Blue
        (75, 0, 130),   # Indigo
        (238, 130, 238) # Violet
    ]

    # Assign colors to each pixel horizontally
    color_width = width // len(colors)
    for i, (r, g, b) in enumerate(colors):
        for x in range(i * color_width, (i + 1) * color_width):
            for y in range(height):
                image[y][x] = f'{r} {g} {b}'

    # Write the image data to a PPM file
    with open('rainbow1.ppm', 'w') as file:
        file.write(f'P3\n{width} {height}\n255\n')
        for row in image:
            file.write(' '.join(row) + '\n')



# Create the rainbow PPM file
create_rainbow_ppm(700, 200)
main()
