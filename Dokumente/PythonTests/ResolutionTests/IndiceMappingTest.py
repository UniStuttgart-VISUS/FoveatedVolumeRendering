import numpy as np

class vec2:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y


class vec3:
    def __init__(self, x=0, y=0, z= 0):
        self.x = x
        self.y = y
        self.z = z


def index_from_2d(coord, m):
    return coord.y * m + coord.x


def coord_from_1d(index, m):
    return vec2(index % m, index / m)


def mapped_index(old_index, m, g):
    return index_from_2d(vec2(old_index * g % m, ((old_index * g) / m) * g), m)


def mapped_2d_index_from_1d(old_index, m, g):
    return vec2(old_index * g % m, ((old_index * g) / m) * g)


def old_2d_to_new_2d_coord(old_coord, m, g, old_m):
    return mapped_2d_index_from_1d(index_from_2d(old_coord, old_m), m, g)


def main():
    g_values = vec3(4,2,1)
    tex_width = 16
    tex_height = 9

    ell2 = vec2(0.5 * tex_width, 0.4 * tex_height)
    ell1 = vec2(0.3 * tex_width, 0.2 * tex_height)

    dim_c = vec2(float(tex_width) / float(g_values.x))
    dim_b = vec2(float(ell2.x) / float(g_values.y), float(ell2.y) / float(g_values.y))
    dim_a = vec2(float(ell1.x) / float(g_values.z), float(ell1.y) / float(g_values.z))

    a_c = dim_c.x * dim_c.y
    a_b = dim_b.x * dim_b.y + a_c
    a_a = dim_a.x * dim_a.y + a_b

    total = a_a

    width = int(np.ceil(np.sqrt(total)) + 1)
    height = int(np.ceil(np.sqrt(total)) + 1)

    for x in range(width):
        for y in range(height):
            coord = vec2(x, y)
            index_1d = index_from_2d(coord, width)
            if index_1d < a_c:
                area = 'C'
                globalId = mapped_2d_index_from_1d(index_1d, tex_width, g_values.x)
                globalId = vec2(globalId.x - (globalId.x % g_values.x), globalId.y - (globalId % g_values.x))
            elif index_1d < a_b:
                area = 'B'
                globalId = mapped_2d_index_from_1d(index_1d, tex_width, g_values.y)
                globalId = vec2(globalId.x - (globalId.x % g_values.y), globalId.y - (globalId.y % g_values.y))
            else:
                area = 'A'
                globalId = mapped_2d_index_from_1d(index_1d, tex_width, g_values.z)
                globalId = vec2(globalId.x - (globalId.x % g_values.z), globalId.y - (globalId.y % g_values.z))
            print("width: " + str(width) + " height: " + str(height) + " Area: " + area + " index_1d: " + str(index_1d) + " globalId: (" + str(globalId.x) + ", " + str(globalId.y) + ")")
    pass


if __name__ == '__main__':
    main()