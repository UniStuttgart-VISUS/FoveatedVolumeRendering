import numpy as np
import matplotlib.pyplot as plt


class window_plotting:
    def __init__(self, width=20, height=20, test=0):
        self.width = width
        self.height = height
        self.resolution = (self.width, self.height)
        self.window = np.ndarray(shape=self.resolution, dtype=tuple, order='C')
        self.reset_window(width, height, test)

    def reset_window(self, width, height, test=0):
        self.width = width
        self.height = height
        self.resolution = (self.width, self.height)
        self.window = np.ndarray(shape=self.resolution, dtype=tuple, order='C')

        # init window values
        for x in range(self.width):
            for y in range(self.height):
                self.window[x, y] = (x, y) if test==0 else (x / float(self.width), y / float(self.height))

    def x_array(self):
        x = []
        for t_x in range(self.width):
            for t_y in range(self.height):
                x += [self.window[t_x, t_y][0]]
        return x

    def y_array(self):
        y = []
        for t_x in range(self.width):
            for t_y in range(self.height):
                y += [self.window[t_x, t_y][1]]
        return y

    def modify_tuples(self, func, *args):
        for x in range(self.width):
            for y in range(self.height):
                self.window[x, y] = func(self.window[x,y], self.width, self.height, *args)


def print_ftp(tp0, tp1):
    if tp0[0] == 0 and tp0[1] == 0:
        print(tp0, tp1)


def tp_mults(v, s):
    return v[0] * s, v[1] * s


def tp_mult(v0, v1):
    return v0[0] * v1[0], v0[1] * v1[1]


def tp_plus(v0, v1):
    return v0[0] + v1[0], v0[1] + v1[1]


def tp_minus(v0, v1):
    return v0[0] - v1[0], v0[1] - v1[1]


def tp_length(v):
    return np.sqrt(pow(v[0], 2) + pow(v[1], 2))


def tp_distance(v0,v1):
    return tp_length(tp_minus(v0, v1))


def harmonic_sinus(t, y, T, phi):
    omega = 2.0 * np.pi / T
    return y * np.sin(omega * t + phi)


def assembled_sinus(x, p):
    if x <= p:
        t = x
        y = p
        T = 4 * p
        if T == 0:
            T = 0.0000001
        phi = 0.0
        y_value = harmonic_sinus(t, y, T, phi)
    else:
        t = x
        y = 1 - p
        T = 4 * (1 - p)
        if T == 0:
            T = 0.0000001
        omega = 2.0 * np.pi / T
        phi = -omega
        y_value = harmonic_sinus(t, y, T, phi) + 1
    return y_value


def straight_line(m, x, d):
    return m * x + d

'''
x is the original x value which is to be changed
p is the point where the mouse is located (one dimensional)
rad is the radius of the area to render with different resolution
ppf is the factor to multiply to pp when calculation the amount of points in the cursors area
'''
def change_density(x, p, rad=0.07, ppf=3.0):
    if x == 0:
        return 0

    l = p - rad
    mid = 2 * rad
    r = 1 - l - mid

    if r == 0:
        print("r == 0 !")
    elif ((l / r) + 1.0) == 0:
        print("((l / r) + 1.0) == 0 !")

    pp = 2*rad*ppf
    one_minus_pp = 1.0 - pp
    one_div_l_div_r_plus_1 = 1.0 / ((l / r) + 1.0)

    lrp = one_minus_pp * (1.0 - one_div_l_div_r_plus_1)
    rrp = one_minus_pp * one_div_l_div_r_plus_1

    if lrp == 0:
        print("lrp == 0 !")
    elif rrp == 0:
        print("rrp == 0 !")
    elif pp == 0:
        print("pp == 0 !")

    lm = l / lrp
    mm = mid / pp
    rm = r / rrp

    ld = 0
    md = lrp * lm - lrp * mm
    rd = ((lrp + pp) * mm + md) - (lrp + pp) * rm

    if x < lrp:
        print("left!")
        m = lm
        d = ld
        y_value = straight_line(m, x, d)
    elif x > lrp + pp:
        print("right!")
        m = rm
        d = rd
        y_value = straight_line(m, x, d)
    else:
        print("middle!")
        m = mm
        d = md
        y_value = straight_line(m, x, d)
    # print("x: {}, y_value: {}, m: {}".format(x, y_value, m))
    # print("lrp: {}, rrp: {}, pp: {}, rp: {}, l: {}, md: {}, r: {}, x: {}, y_value: {}, m: {}, p: {}, rad: {}, ires: {}".format(lrp, rrp, pp, rp, l, md, r, x, y_value, m, p, rad, ires))
    return y_value
    pass

''' 
    This has some flaws. E.g. the density of the points left and right of the
    of the given point p is different 
'''
def mid_linear(x, p, rad=0.07, ires=0.5):
    if p-rad == 0:
        return 0
    if x < p-rad:
        left_p = p - rad * ires
        m = left_p / float(p-rad)
        d = 0
        y_value = straight_line(m, x, d)
    elif x > p+rad:
        right_p = p + rad * ires
        m = (1-right_p) / float(1-p-rad)
        d = - (m * (p+rad) - (p + ires * rad))
        y_value = straight_line(m, x, d)
    else:
        m = ires
        d = ires * p
        y_value = straight_line(m, x, d)
    return y_value


def test():
    rad = 0.05
    p = 0.2
    l = p - rad
    md = 2 * rad
    r = 1 - l - md
    ppf = 2

    ges = 1
    pp = 2*rad*ppf
    one_minus_pp = 1.0 - pp
    one_div_l_div_r_plus_1 = 1.0 / ((l / r) + 1.0)

    lrp = one_minus_pp * (1.0 - one_div_l_div_r_plus_1)
    rrp = one_minus_pp * one_div_l_div_r_plus_1

    lm = l / lrp
    rm = r / rrp
    mm = md / pp

    print("l: {}, r: {}, p: {}, rad: {}, ppf: {}, pp: {}, lrp: {}, rrp: {}".format(l, r, p, rad, ppf, pp, lrp, rrp))


def identity(tuple_element, width, height):
    return tuple_element


def npos(tuple_element, width, height, point,  func):
    # print_ftp(tuple_element, point)
    conv_x = tuple_element[0] / width
    conv_y = tuple_element[1] / height
    new_x = func(conv_x, point[0])
    new_y = func(conv_y, point[1])

    return width * new_x, height * new_y


def npos_test(tuple_element, width, height, point, func, *args):
    # conv_x = tuple_element[0] / width
    # conv_y = tuple_element[1] / height
    new_x = func(tuple_element[0], point[0], *args)
    new_y = func(tuple_element[1], point[1], *args)
    return new_x, 0.0
    #return tuple_element[0], new_x


def log2d(tuple_element):
    return np.log(tuple_element[0]), np.log(tuple_element[1])


def sqrt2d(tuple_element):
    return np.sqrt(tuple_element[0]), np.sqrt(tuple_element[1])


def pow2d(tuple_element, exponent):
    return np.power(tuple_element[0], exponent), np.power(tuple_element[1], exponent)


def main():
    wp = window_plotting(20, 1, 1)
    point = (0.3, 0.4)
    rad = 0.07
    inc_res_factor = 2.5
    wp.modify_tuples(npos_test, point, change_density, rad, inc_res_factor)

    axes = plt.gca()
    width = 1#wp.width
    height = 1#wp.height
    axes.set_xlim([-0.1, 1.1 * width])
    axes.set_ylim([-2.1, 2.1 * height])
    axes.set_aspect('equal')

    x_v = wp.x_array()
    y_v = wp.y_array()

    # print(x_v, y_v)

    plt.scatter(x_v, y_v, s=1)

    plt.show()
    # test()
    return 0


if __name__ == "__main__":
    main()