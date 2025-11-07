import math
from typing import Sequence

def angle_between(u: Sequence[float], v: Sequence[float]) -> float:
    """
    Returns the unsigned angle between 2D vectors u and v in radians.
    Equivalent to the GLSL snippet using dot/length + clamp + acos.
    """
    ux, uy = float(u[0]), float(u[1])
    vx, vy = float(v[0]), float(v[1])

    dot_prod = ux * vx + uy * vy
    len_u = math.hypot(ux, uy)
    len_v = math.hypot(vx, vy)

    if len_u == 0.0 or len_v == 0.0:
        raise ValueError("angle_between: zero-length vector")

    cos_theta = dot_prod / (len_u * len_v)
    # clamp to [-1, 1] to avoid NaNs from tiny floating-point overshoots
    cos_theta = max(-1.0, min(1.0, cos_theta))
    return math.acos(cos_theta)

# quick test
if __name__ == "__main__":
    print(angle_between((1, 0), (0, 1)))      # ~1.57079632679 (π/2)
    print(angle_between((1, 0), (1, 0)))      # 0.0
    print(angle_between((1, 0), (-1, 0)))     # ~3.14159265359 (π)
