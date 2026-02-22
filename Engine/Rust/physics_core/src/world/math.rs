use std::ops::{Add, AddAssign, Mul, Sub, SubAssign};

use super::types::{RexQuat, RexVec3};

#[derive(Clone, Copy, Default, Debug)]
pub(crate) struct Vec3 {
    pub(crate) x: f32,
    pub(crate) y: f32,
    pub(crate) z: f32,
}

impl Vec3 {
    pub(crate) fn dot(self, rhs: Vec3) -> f32 {
        self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
    }

    pub(crate) fn cross(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.y * rhs.z - self.z * rhs.y,
            y: self.z * rhs.x - self.x * rhs.z,
            z: self.x * rhs.y - self.y * rhs.x,
        }
    }

    pub(crate) fn length_sq(self) -> f32 {
        self.dot(self)
    }

    pub(crate) fn normalized(self) -> Vec3 {
        let len_sq = self.length_sq();
        if len_sq <= 1.0e-8 {
            return Vec3::default();
        }
        let inv = 1.0 / len_sq.sqrt();
        self * inv
    }
}

impl Add for Vec3 {
    type Output = Vec3;

    fn add(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z,
        }
    }
}

impl Sub for Vec3 {
    type Output = Vec3;

    fn sub(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
            z: self.z - rhs.z,
        }
    }
}

impl Mul<f32> for Vec3 {
    type Output = Vec3;

    fn mul(self, rhs: f32) -> Vec3 {
        Vec3 {
            x: self.x * rhs,
            y: self.y * rhs,
            z: self.z * rhs,
        }
    }
}

impl AddAssign for Vec3 {
    fn add_assign(&mut self, rhs: Vec3) {
        self.x += rhs.x;
        self.y += rhs.y;
        self.z += rhs.z;
    }
}

impl SubAssign for Vec3 {
    fn sub_assign(&mut self, rhs: Vec3) {
        self.x -= rhs.x;
        self.y -= rhs.y;
        self.z -= rhs.z;
    }
}

#[derive(Clone, Copy, Default)]
pub(crate) struct Quat {
    pub(crate) x: f32,
    pub(crate) y: f32,
    pub(crate) z: f32,
    pub(crate) w: f32,
}

impl Quat {
    pub(crate) fn identity() -> Quat {
        Quat {
            x: 0.0,
            y: 0.0,
            z: 0.0,
            w: 1.0,
        }
    }

    pub(crate) fn normalized(self) -> Quat {
        let len_sq = self.x * self.x + self.y * self.y + self.z * self.z + self.w * self.w;
        if len_sq <= 1.0e-12 {
            return Quat::identity();
        }
        let inv = 1.0 / len_sq.sqrt();
        Quat {
            x: self.x * inv,
            y: self.y * inv,
            z: self.z * inv,
            w: self.w * inv,
        }
    }

    pub(crate) fn conjugate(self) -> Quat {
        Quat {
            x: -self.x,
            y: -self.y,
            z: -self.z,
            w: self.w,
        }
    }

    pub(crate) fn mul(self, rhs: Quat) -> Quat {
        Quat {
            x: self.w * rhs.x + self.x * rhs.w + self.y * rhs.z - self.z * rhs.y,
            y: self.w * rhs.y - self.x * rhs.z + self.y * rhs.w + self.z * rhs.x,
            z: self.w * rhs.z + self.x * rhs.y - self.y * rhs.x + self.z * rhs.w,
            w: self.w * rhs.w - self.x * rhs.x - self.y * rhs.y - self.z * rhs.z,
        }
    }

    pub(crate) fn rotate(self, v: Vec3) -> Vec3 {
        let q = self.normalized();
        let qv = Quat {
            x: v.x,
            y: v.y,
            z: v.z,
            w: 0.0,
        };
        let qr = q.mul(qv).mul(q.conjugate());
        Vec3 {
            x: qr.x,
            y: qr.y,
            z: qr.z,
        }
    }
}

impl From<RexVec3> for Vec3 {
    fn from(value: RexVec3) -> Vec3 {
        Vec3 {
            x: value.x,
            y: value.y,
            z: value.z,
        }
    }
}

impl From<Vec3> for RexVec3 {
    fn from(value: Vec3) -> RexVec3 {
        RexVec3 {
            x: value.x,
            y: value.y,
            z: value.z,
        }
    }
}

impl From<RexQuat> for Quat {
    fn from(value: RexQuat) -> Quat {
        Quat {
            x: value.x,
            y: value.y,
            z: value.z,
            w: value.w,
        }
    }
}

impl From<Quat> for RexQuat {
    fn from(value: Quat) -> RexQuat {
        RexQuat {
            x: value.x,
            y: value.y,
            z: value.z,
            w: value.w,
        }
    }
}
