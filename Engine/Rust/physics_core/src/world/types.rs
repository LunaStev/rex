#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct RexVec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct RexQuat {
    pub x: f32,
    pub y: f32,
    pub z: f32,
    pub w: f32,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct RexPhysicsBody {
    pub id: u64,
    pub body_type: u32,
    pub flags: u32,
    pub mass: f32,
    pub inv_mass: f32,

    pub position: RexVec3,
    pub scale: RexVec3,
    pub orientation: RexQuat,

    pub velocity: RexVec3,
    pub angular_velocity: RexVec3,
    pub force: RexVec3,
    pub torque: RexVec3,

    pub inv_inertia: RexVec3,
    pub local_bounds_min: RexVec3,
    pub local_bounds_max: RexVec3,

    pub restitution: f32,
    pub static_friction: f32,
    pub dynamic_friction: f32,
    pub linear_damping: f32,
    pub angular_damping: f32,

    pub is_awake: u32,
    pub sleep_timer: f32,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct RexDistanceJoint {
    pub id: u64,
    pub body_a: u64,
    pub body_b: u64,
    pub local_anchor_a: RexVec3,
    pub local_anchor_b: RexVec3,
    pub rest_length: f32,
    pub stiffness: f32,
    pub damping: f32,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct RexRaycastHit {
    pub hit: u32,
    pub body: u64,
    pub point: RexVec3,
    pub normal: RexVec3,
    pub distance: f32,
}

pub(crate) const BODY_TYPE_DYNAMIC: u32 = 1;

pub(crate) const BODY_FLAG_ENABLE_CCD: u32 = 1 << 0;
pub(crate) const BODY_FLAG_ENABLE_SLEEP: u32 = 1 << 1;
