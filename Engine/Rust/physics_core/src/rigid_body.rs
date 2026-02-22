use crate::world::{RexPhysicsBody, RexQuat, RexVec3};

const BODY_TYPE_DYNAMIC: u32 = 1;

#[derive(Clone, Copy, Default)]
struct Vec3 {
    x: f32,
    y: f32,
    z: f32,
}

impl Vec3 {
    fn cross(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.y * rhs.z - self.z * rhs.y,
            y: self.z * rhs.x - self.x * rhs.z,
            z: self.x * rhs.y - self.y * rhs.x,
        }
    }

}

impl std::ops::Add for Vec3 {
    type Output = Vec3;

    fn add(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z,
        }
    }
}

impl std::ops::Sub for Vec3 {
    type Output = Vec3;

    fn sub(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
            z: self.z - rhs.z,
        }
    }
}

impl std::ops::AddAssign for Vec3 {
    fn add_assign(&mut self, rhs: Vec3) {
        self.x += rhs.x;
        self.y += rhs.y;
        self.z += rhs.z;
    }
}

impl std::ops::Mul<f32> for Vec3 {
    type Output = Vec3;

    fn mul(self, rhs: f32) -> Vec3 {
        Vec3 {
            x: self.x * rhs,
            y: self.y * rhs,
            z: self.z * rhs,
        }
    }
}

impl From<RexVec3> for Vec3 {
    fn from(value: RexVec3) -> Self {
        Vec3 {
            x: value.x,
            y: value.y,
            z: value.z,
        }
    }
}

impl From<Vec3> for RexVec3 {
    fn from(value: Vec3) -> Self {
        RexVec3 {
            x: value.x,
            y: value.y,
            z: value.z,
        }
    }
}

#[derive(Clone, Copy, Default)]
struct Quat {
    x: f32,
    y: f32,
    z: f32,
    w: f32,
}

impl Quat {
    fn identity() -> Quat {
        Quat {
            x: 0.0,
            y: 0.0,
            z: 0.0,
            w: 1.0,
        }
    }

    fn normalized(self) -> Quat {
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

    fn conjugate(self) -> Quat {
        Quat {
            x: -self.x,
            y: -self.y,
            z: -self.z,
            w: self.w,
        }
    }

    fn mul(self, rhs: Quat) -> Quat {
        Quat {
            x: self.w * rhs.x + self.x * rhs.w + self.y * rhs.z - self.z * rhs.y,
            y: self.w * rhs.y - self.x * rhs.z + self.y * rhs.w + self.z * rhs.x,
            z: self.w * rhs.z + self.x * rhs.y - self.y * rhs.x + self.z * rhs.w,
            w: self.w * rhs.w - self.x * rhs.x - self.y * rhs.y - self.z * rhs.z,
        }
    }

    fn rotate(self, v: Vec3) -> Vec3 {
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

impl From<RexQuat> for Quat {
    fn from(value: RexQuat) -> Self {
        Quat {
            x: value.x,
            y: value.y,
            z: value.z,
            w: value.w,
        }
    }
}

fn is_dynamic(body: &RexPhysicsBody) -> bool {
    body.body_type == BODY_TYPE_DYNAMIC && body.inv_mass > 0.0
}

fn wake_up(body: &mut RexPhysicsBody) {
    body.is_awake = 1;
    body.sleep_timer = 0.0;
}

fn mul_world_inv_inertia(body: &RexPhysicsBody, v: Vec3) -> Vec3 {
    let q = Quat::from(body.orientation).normalized();
    let local = q.conjugate().rotate(v);
    let local_applied = Vec3 {
        x: body.inv_inertia.x * local.x,
        y: body.inv_inertia.y * local.y,
        z: body.inv_inertia.z * local.z,
    };
    q.rotate(local_applied)
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_set_mass(body: *mut RexPhysicsBody, mass: f32) {
    let Some(body) = body.as_mut() else {
        return;
    };

    body.mass = mass;
    body.inv_mass = if body.body_type == BODY_TYPE_DYNAMIC && mass > 0.0 {
        1.0 / mass
    } else {
        0.0
    };

    rex_rigidbody_update_inertia_tensor(body as *mut RexPhysicsBody);
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_apply_force(body: *mut RexPhysicsBody, force: RexVec3) {
    let Some(body) = body.as_mut() else {
        return;
    };
    if !is_dynamic(body) {
        return;
    }

    wake_up(body);
    body.force.x += force.x;
    body.force.y += force.y;
    body.force.z += force.z;
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_apply_impulse(body: *mut RexPhysicsBody, impulse: RexVec3) {
    let Some(body) = body.as_mut() else {
        return;
    };
    if !is_dynamic(body) {
        return;
    }

    wake_up(body);
    body.velocity.x += impulse.x * body.inv_mass;
    body.velocity.y += impulse.y * body.inv_mass;
    body.velocity.z += impulse.z * body.inv_mass;
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_apply_torque(body: *mut RexPhysicsBody, torque: RexVec3) {
    let Some(body) = body.as_mut() else {
        return;
    };
    if !is_dynamic(body) {
        return;
    }

    wake_up(body);
    body.torque.x += torque.x;
    body.torque.y += torque.y;
    body.torque.z += torque.z;
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_apply_angular_impulse(body: *mut RexPhysicsBody, impulse: RexVec3) {
    let Some(body) = body.as_mut() else {
        return;
    };
    if !is_dynamic(body) {
        return;
    }

    wake_up(body);
    let delta = mul_world_inv_inertia(body, impulse.into());
    body.angular_velocity.x += delta.x;
    body.angular_velocity.y += delta.y;
    body.angular_velocity.z += delta.z;
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_apply_impulse_at_point(
    body: *mut RexPhysicsBody,
    impulse: RexVec3,
    world_point: RexVec3,
) {
    let Some(body) = body.as_mut() else {
        return;
    };
    if !is_dynamic(body) {
        return;
    }

    wake_up(body);

    body.velocity.x += impulse.x * body.inv_mass;
    body.velocity.y += impulse.y * body.inv_mass;
    body.velocity.z += impulse.z * body.inv_mass;

    let r = Vec3 {
        x: world_point.x - body.position.x,
        y: world_point.y - body.position.y,
        z: world_point.z - body.position.z,
    };
    let angular_impulse = r.cross(impulse.into());
    let delta = mul_world_inv_inertia(body, angular_impulse);
    body.angular_velocity.x += delta.x;
    body.angular_velocity.y += delta.y;
    body.angular_velocity.z += delta.z;
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_update_inertia_tensor(body: *mut RexPhysicsBody) {
    let Some(body) = body.as_mut() else {
        return;
    };

    if body.body_type != BODY_TYPE_DYNAMIC || body.mass <= 0.0 {
        body.inv_inertia = RexVec3::default();
        return;
    }

    let width = ((body.local_bounds_max.x - body.local_bounds_min.x) * body.scale.x)
        .abs()
        .max(0.001);
    let height = ((body.local_bounds_max.y - body.local_bounds_min.y) * body.scale.y)
        .abs()
        .max(0.001);
    let depth = ((body.local_bounds_max.z - body.local_bounds_min.z) * body.scale.z)
        .abs()
        .max(0.001);

    let ixx = (body.mass / 12.0) * (height * height + depth * depth);
    let iyy = (body.mass / 12.0) * (width * width + depth * depth);
    let izz = (body.mass / 12.0) * (width * width + height * height);

    body.inv_inertia.x = if ixx > 1.0e-6 { 1.0 / ixx } else { 0.0 };
    body.inv_inertia.y = if iyy > 1.0e-6 { 1.0 / iyy } else { 0.0 };
    body.inv_inertia.z = if izz > 1.0e-6 { 1.0 / izz } else { 0.0 };
}

#[no_mangle]
pub unsafe extern "C" fn rex_rigidbody_wake_up(body: *mut RexPhysicsBody) {
    let Some(body) = body.as_mut() else {
        return;
    };
    wake_up(body);
}
