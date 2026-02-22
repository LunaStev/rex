use std::collections::HashMap;
use std::ops::{Add, AddAssign, Mul, Sub, SubAssign};

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

const BODY_TYPE_DYNAMIC: u32 = 1;

const BODY_FLAG_ENABLE_CCD: u32 = 1 << 0;
const BODY_FLAG_ENABLE_SLEEP: u32 = 1 << 1;

#[derive(Clone, Copy, Default, Debug)]
struct Vec3 {
    x: f32,
    y: f32,
    z: f32,
}

impl Vec3 {
    fn dot(self, rhs: Vec3) -> f32 {
        self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
    }

    fn cross(self, rhs: Vec3) -> Vec3 {
        Vec3 {
            x: self.y * rhs.z - self.z * rhs.y,
            y: self.z * rhs.x - self.x * rhs.z,
            z: self.x * rhs.y - self.y * rhs.x,
        }
    }

    fn length_sq(self) -> f32 {
        self.dot(self)
    }

    fn normalized(self) -> Vec3 {
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

#[derive(Clone)]
struct SimBody {
    id: u64,
    body_type: u32,
    flags: u32,
    inv_mass: f32,

    position: Vec3,
    scale: Vec3,
    orientation: Quat,

    velocity: Vec3,
    angular_velocity: Vec3,
    force: Vec3,
    torque: Vec3,

    inv_inertia: Vec3,
    local_bounds_min: Vec3,
    local_bounds_max: Vec3,

    restitution: f32,
    static_friction: f32,
    dynamic_friction: f32,
    linear_damping: f32,
    angular_damping: f32,

    is_awake: bool,
    sleep_timer: f32,
}

impl SimBody {
    fn from_ffi(raw: &RexPhysicsBody) -> SimBody {
        SimBody {
            id: raw.id,
            body_type: raw.body_type,
            flags: raw.flags,
            inv_mass: raw.inv_mass,

            position: raw.position.into(),
            scale: raw.scale.into(),
            orientation: raw.orientation.into(),

            velocity: raw.velocity.into(),
            angular_velocity: raw.angular_velocity.into(),
            force: raw.force.into(),
            torque: raw.torque.into(),

            inv_inertia: raw.inv_inertia.into(),
            local_bounds_min: raw.local_bounds_min.into(),
            local_bounds_max: raw.local_bounds_max.into(),

            restitution: raw.restitution,
            static_friction: raw.static_friction,
            dynamic_friction: raw.dynamic_friction,
            linear_damping: raw.linear_damping,
            angular_damping: raw.angular_damping,

            is_awake: raw.is_awake != 0,
            sleep_timer: raw.sleep_timer,
        }
    }

    fn write_back(&self, raw: &mut RexPhysicsBody) {
        raw.position = self.position.into();
        raw.orientation = self.orientation.normalized().into();
        raw.velocity = self.velocity.into();
        raw.angular_velocity = self.angular_velocity.into();
        raw.force = self.force.into();
        raw.torque = self.torque.into();
        raw.is_awake = if self.is_awake { 1 } else { 0 };
        raw.sleep_timer = self.sleep_timer;
    }

    fn is_dynamic(&self) -> bool {
        self.body_type == BODY_TYPE_DYNAMIC && self.inv_mass > 0.0
    }

    fn enable_ccd(&self) -> bool {
        (self.flags & BODY_FLAG_ENABLE_CCD) != 0
    }

    fn enable_sleep(&self) -> bool {
        (self.flags & BODY_FLAG_ENABLE_SLEEP) != 0
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

#[derive(Clone, Copy)]
struct Obb {
    center: Vec3,
    axis: [Vec3; 3],
    half_extents: Vec3,
}

#[derive(Clone, Copy)]
struct BoundingSphere {
    center: Vec3,
    radius: f32,
}

#[derive(Clone, Copy)]
struct ManifoldPoint {
    point: Vec3,
    penetration: f32,
    r_a: Vec3,
    r_b: Vec3,
    tangent: Vec3,
    normal_mass: f32,
    tangent_mass: f32,
    bias: f32,
    normal_impulse: f32,
    tangent_impulse: f32,
}

impl Default for ManifoldPoint {
    fn default() -> Self {
        ManifoldPoint {
            point: Vec3::default(),
            penetration: 0.0,
            r_a: Vec3::default(),
            r_b: Vec3::default(),
            tangent: Vec3 { x: 1.0, y: 0.0, z: 0.0 },
            normal_mass: 0.0,
            tangent_mass: 0.0,
            bias: 0.0,
            normal_impulse: 0.0,
            tangent_impulse: 0.0,
        }
    }
}

struct ContactManifold {
    a_idx: usize,
    b_idx: usize,
    key: u64,
    normal: Vec3,
    static_friction: f32,
    dynamic_friction: f32,
    points: Vec<ManifoldPoint>,
}

#[derive(Clone, Copy, Default)]
struct CachedManifoldPoint {
    point: Vec3,
    normal_impulse: f32,
    tangent_impulse: f32,
}

#[derive(Clone)]
struct ManifoldCacheEntry {
    normal: Vec3,
    points: [CachedManifoldPoint; 4],
    point_count: usize,
    last_touched_frame: u32,
}

impl Default for ManifoldCacheEntry {
    fn default() -> Self {
        ManifoldCacheEntry {
            normal: Vec3 { x: 0.0, y: 1.0, z: 0.0 },
            points: [CachedManifoldPoint::default(); 4],
            point_count: 0,
            last_touched_frame: 0,
        }
    }
}

struct JointConstraint {
    id: u64,
    a_idx: usize,
    b_idx: usize,
    local_anchor_a: Vec3,
    local_anchor_b: Vec3,
    rest_length: f32,
    stiffness: f32,
    damping: f32,

    axis: Vec3,
    r_a: Vec3,
    r_b: Vec3,
    bias: f32,
    effective_mass: f32,
    accumulated_impulse: f32,
}

pub struct RexPhysicsWorld {
    gravity: Vec3,
    solver_iterations: i32,
    position_iterations: i32,
    max_sub_steps: i32,

    max_frame_step: f32,
    penetration_slop: f32,
    position_correction: f32,
    restitution_threshold: f32,
    baumgarte: f32,
    warm_start_scale: f32,
    cache_persistence_frames: u32,

    frame_counter: u32,
    accumulator: f32,

    manifold_cache: HashMap<u64, ManifoldCacheEntry>,
    joint_impulse_cache: HashMap<u64, f32>,
}

impl RexPhysicsWorld {
    fn new() -> RexPhysicsWorld {
        RexPhysicsWorld {
            gravity: Vec3 {
                x: 0.0,
                y: -9.81,
                z: 0.0,
            },
            solver_iterations: 10,
            position_iterations: 4,
            max_sub_steps: 6,

            max_frame_step: 0.1,
            penetration_slop: 0.005,
            position_correction: 0.75,
            restitution_threshold: 1.0,
            baumgarte: 0.22,
            warm_start_scale: 0.95,
            cache_persistence_frames: 45,

            frame_counter: 0,
            accumulator: 0.0,

            manifold_cache: HashMap::new(),
            joint_impulse_cache: HashMap::new(),
        }
    }

    fn step(&mut self, dt: f32, bodies: &mut [SimBody], joints_raw: &[RexDistanceJoint]) {
        if dt <= 0.0 || bodies.is_empty() {
            return;
        }

        let dt = dt.min(self.max_frame_step);
        self.accumulator += dt;

        let mut fixed_steps = 0;
        let max_fixed_steps_per_frame = 8;
        let fixed_step = 1.0 / 60.0;

        while self.accumulator >= fixed_step && fixed_steps < max_fixed_steps_per_frame {
            self.simulate(fixed_step, bodies, joints_raw);
            self.accumulator -= fixed_step;
            fixed_steps += 1;
        }

        if fixed_steps == max_fixed_steps_per_frame {
            self.accumulator = 0.0;
        }
    }

    fn simulate(&mut self, dt: f32, bodies: &mut [SimBody], joints_raw: &[RexDistanceJoint]) {
        let sub_steps = self.compute_adaptive_sub_steps(dt, bodies);
        let sub_dt = dt / sub_steps as f32;

        let mut body_by_id = HashMap::with_capacity(bodies.len());
        for (i, b) in bodies.iter().enumerate() {
            body_by_id.insert(b.id, i);
        }

        for _ in 0..sub_steps {
            self.frame_counter = self.frame_counter.wrapping_add(1);

            let start_positions: Vec<Vec3> = bodies.iter().map(|b| b.position).collect();
            let start_orientations: Vec<Quat> = bodies.iter().map(|b| b.orientation).collect();

            self.integrate(sub_dt, bodies);

            let mut pairs = self.build_broadphase_pairs(sub_dt, bodies);
            self.apply_toi_ccd(
                sub_dt,
                bodies,
                &pairs,
                &start_positions,
                &start_orientations,
            );

            pairs = self.build_broadphase_pairs(sub_dt, bodies);

            let mut manifolds = Vec::with_capacity(pairs.len());
            for (a_idx, b_idx) in pairs {
                if let Some(m) = self.build_manifold(bodies, a_idx, b_idx) {
                    manifolds.push(m);
                }
            }

            let mut joints = self.prepare_joint_constraints(joints_raw, &body_by_id, bodies, sub_dt);

            self.prepare_manifolds(bodies, &mut manifolds, sub_dt);
            self.warm_start_manifolds(bodies, &manifolds);
            self.warm_start_joints(bodies, &joints);

            for _ in 0..self.solver_iterations.max(1) {
                self.solve_joint_velocity_constraints(bodies, &mut joints);
                self.solve_velocity_constraints(bodies, &mut manifolds);
            }

            for _ in 0..self.position_iterations.max(1) {
                self.solve_joint_position_constraints(bodies, &joints);
                self.solve_position_constraints(bodies, &manifolds);
            }

            self.cache_manifolds(&manifolds);
            self.cache_joint_impulses(&joints);
            self.prune_manifold_cache();
            self.prune_joint_cache();
            self.update_sleep_states(sub_dt, bodies);
        }
    }

    fn compute_adaptive_sub_steps(&self, dt: f32, bodies: &[SimBody]) -> i32 {
        let mut max_motion = 0.0f32;
        let mut min_half_extent = f32::MAX;

        for b in bodies {
            if !b.is_dynamic() {
                continue;
            }

            let size_x = ((b.local_bounds_max.x - b.local_bounds_min.x) * b.scale.x).abs();
            let size_y = ((b.local_bounds_max.y - b.local_bounds_min.y) * b.scale.y).abs();
            let size_z = ((b.local_bounds_max.z - b.local_bounds_min.z) * b.scale.z).abs();
            let min_extent = size_x.min(size_y).min(size_z).max(0.001);
            min_half_extent = min_half_extent.min(min_extent * 0.5);

            let radius = 0.5 * (size_x * size_x + size_y * size_y + size_z * size_z).sqrt();
            let linear_speed = b.velocity.length_sq().sqrt();
            let angular_speed = b.angular_velocity.length_sq().sqrt();
            let estimated = (linear_speed + angular_speed * radius) * dt;
            max_motion = max_motion.max(estimated);
        }

        if min_half_extent == f32::MAX {
            return 1;
        }

        let safe_motion = (min_half_extent * 0.35).max(0.001);
        let recommended = (max_motion / safe_motion).ceil() as i32;
        recommended.clamp(1, self.max_sub_steps.max(1))
    }

    fn integrate(&self, dt: f32, bodies: &mut [SimBody]) {
        for b in bodies.iter_mut() {
            if !b.is_dynamic() {
                continue;
            }

            if !b.is_awake {
                if b.force.length_sq() > 1.0e-6 || b.torque.length_sq() > 1.0e-6 {
                    b.is_awake = true;
                    b.sleep_timer = 0.0;
                } else {
                    continue;
                }
            }

            let mut accel = self.gravity;
            if b.inv_mass > 0.0 {
                accel += b.force * b.inv_mass;
            }
            let angular_accel = mul_inv_inertia(b, b.torque);

            b.velocity += accel * dt;
            b.angular_velocity += angular_accel * dt;

            let linear_damping = (1.0 - b.linear_damping * dt).max(0.0);
            let angular_damping = (1.0 - b.angular_damping * dt).max(0.0);
            b.velocity = b.velocity * linear_damping;
            b.angular_velocity = b.angular_velocity * angular_damping;

            b.position += b.velocity * dt;
            b.orientation = integrate_orientation(b.orientation, b.angular_velocity, dt);

            b.force = Vec3::default();
            b.torque = Vec3::default();
        }
    }

    fn build_broadphase_pairs(&self, dt: f32, bodies: &[SimBody]) -> Vec<(usize, usize)> {
        let mut spheres = Vec::with_capacity(bodies.len());
        for b in bodies {
            let speed = b.velocity.length_sq().sqrt();
            let pad = if b.body_type == BODY_TYPE_DYNAMIC { speed * dt } else { 0.0 };
            spheres.push(self.compute_bounding_sphere(b, b.position, b.orientation, pad));
        }

        let mut pairs = Vec::new();
        for i in 0..bodies.len() {
            for j in (i + 1)..bodies.len() {
                if bodies[i].inv_mass + bodies[j].inv_mass <= 0.0 {
                    continue;
                }
                let d = spheres[j].center - spheres[i].center;
                let r = spheres[i].radius + spheres[j].radius;
                if d.length_sq() <= r * r {
                    pairs.push((i, j));
                }
            }
        }
        pairs
    }

    fn apply_toi_ccd(
        &self,
        dt: f32,
        bodies: &mut [SimBody],
        pairs: &[(usize, usize)],
        start_positions: &[Vec3],
        start_orientations: &[Quat],
    ) {
        if dt <= 0.0 {
            return;
        }

        let mut toi_scale: HashMap<usize, f32> = HashMap::new();
        let mut collision_normals: HashMap<usize, Vec3> = HashMap::new();

        for &(ia, ib) in pairs {
            let a = &bodies[ia];
            let b = &bodies[ib];

            let ccd_a = a.body_type == BODY_TYPE_DYNAMIC && a.enable_ccd();
            let ccd_b = b.body_type == BODY_TYPE_DYNAMIC && b.enable_ccd();
            if !ccd_a && !ccd_b {
                continue;
            }

            let mut toi = 1.0;
            let mut normal = Vec3::default();
            if !self.compute_swept_sphere_toi(
                a,
                start_positions[ia],
                start_orientations[ia],
                a.velocity,
                b,
                start_positions[ib],
                start_orientations[ib],
                b.velocity,
                dt,
                &mut toi,
                &mut normal,
            ) {
                continue;
            }
            if toi >= 1.0 {
                continue;
            }

            if ccd_a {
                toi_scale
                    .entry(ia)
                    .and_modify(|v| *v = (*v).min(toi))
                    .or_insert(toi);
                collision_normals
                    .entry(ia)
                    .and_modify(|n| *n -= normal)
                    .or_insert(normal * -1.0);
            }
            if ccd_b {
                toi_scale
                    .entry(ib)
                    .and_modify(|v| *v = (*v).min(toi))
                    .or_insert(toi);
                collision_normals
                    .entry(ib)
                    .and_modify(|n| *n += normal)
                    .or_insert(normal);
            }
        }

        for (idx, mut scale) in toi_scale {
            scale = clamp01(scale);
            if scale >= 1.0 {
                continue;
            }

            let start_pos = start_positions[idx];
            let start_ori = start_orientations[idx];
            let body = &mut bodies[idx];

            body.position = start_pos + body.velocity * (dt * scale);
            body.orientation = integrate_orientation(start_ori, body.angular_velocity, dt * scale);

            let n = collision_normals
                .get(&idx)
                .copied()
                .unwrap_or_default()
                .normalized();
            if n.length_sq() > 1.0e-8 {
                let vn = body.velocity.dot(n);
                if vn < 0.0 {
                    body.velocity -= n * vn;
                }
            }
            body.is_awake = true;
            body.sleep_timer = 0.0;
        }
    }

    #[allow(clippy::too_many_arguments)]
    fn compute_swept_sphere_toi(
        &self,
        a: &SimBody,
        start_pos_a: Vec3,
        start_orient_a: Quat,
        vel_a: Vec3,
        b: &SimBody,
        start_pos_b: Vec3,
        start_orient_b: Quat,
        vel_b: Vec3,
        dt: f32,
        out_toi: &mut f32,
        out_normal: &mut Vec3,
    ) -> bool {
        if dt <= 0.0 {
            return false;
        }

        let sa = self.compute_bounding_sphere(a, start_pos_a, start_orient_a, 0.0);
        let sb = self.compute_bounding_sphere(b, start_pos_b, start_orient_b, 0.0);

        let rel_start = sb.center - sa.center;
        let rel_disp = (vel_b - vel_a) * dt;
        let radius = sa.radius + sb.radius;

        let c = rel_start.dot(rel_start) - radius * radius;
        if c <= 0.0 {
            return false;
        }

        let a_coef = rel_disp.dot(rel_disp);
        if a_coef <= 1.0e-10 {
            return false;
        }

        let b_coef = 2.0 * rel_start.dot(rel_disp);
        let disc = b_coef * b_coef - 4.0 * a_coef * c;
        if disc < 0.0 {
            return false;
        }

        let sqrt_disc = disc.sqrt();
        let u = (-b_coef - sqrt_disc) / (2.0 * a_coef);
        if !(0.0..=1.0).contains(&u) {
            return false;
        }

        *out_toi = u;
        *out_normal = (rel_start + rel_disp * u).normalized();
        if out_normal.length_sq() <= 1.0e-8 {
            *out_normal = rel_start.normalized();
        }
        out_normal.length_sq() > 1.0e-8
    }

    fn prepare_joint_constraints(
        &self,
        joints_raw: &[RexDistanceJoint],
        body_by_id: &HashMap<u64, usize>,
        bodies: &[SimBody],
        dt: f32,
    ) -> Vec<JointConstraint> {
        if dt <= 0.0 {
            return Vec::new();
        }

        let mut joints = Vec::with_capacity(joints_raw.len());
        for j in joints_raw {
            let Some(&a_idx) = body_by_id.get(&j.body_a) else {
                continue;
            };
            let Some(&b_idx) = body_by_id.get(&j.body_b) else {
                continue;
            };
            if a_idx == b_idx {
                continue;
            }

            let a = &bodies[a_idx];
            let b = &bodies[b_idx];

            let local_anchor_a: Vec3 = j.local_anchor_a.into();
            let local_anchor_b: Vec3 = j.local_anchor_b.into();

            let world_a = a.position + local_anchor_a;
            let world_b = b.position + local_anchor_b;
            let delta = world_b - world_a;
            let len_sq = delta.length_sq();
            let len = if len_sq > 1.0e-10 { len_sq.sqrt() } else { 0.0 };

            let axis = if len > 1.0e-5 {
                delta * (1.0 / len)
            } else {
                Vec3 {
                    x: 1.0,
                    y: 0.0,
                    z: 0.0,
                }
            };
            let r_a = world_a - a.position;
            let r_b = world_b - b.position;

            let inv_mass_a = a.inv_mass;
            let inv_mass_b = b.inv_mass;
            let ra_x_n = r_a.cross(axis);
            let rb_x_n = r_b.cross(axis);
            let ang_a = mul_inv_inertia(a, ra_x_n).cross(r_a).dot(axis);
            let ang_b = mul_inv_inertia(b, rb_x_n).cross(r_b).dot(axis);
            let denom = inv_mass_a + inv_mass_b + ang_a + ang_b;
            let effective_mass = if denom > 1.0e-8 { 1.0 / denom } else { 0.0 };

            let error = len - j.rest_length;
            let bias = (j.stiffness.clamp(0.0, 1.0) / dt) * error;

            let accumulated_impulse = *self.joint_impulse_cache.get(&j.id).unwrap_or(&0.0);

            joints.push(JointConstraint {
                id: j.id,
                a_idx,
                b_idx,
                local_anchor_a,
                local_anchor_b,
                rest_length: j.rest_length,
                stiffness: j.stiffness.clamp(0.0, 1.0),
                damping: j.damping.max(0.0),

                axis,
                r_a,
                r_b,
                bias,
                effective_mass,
                accumulated_impulse,
            });
        }

        joints
    }

    fn warm_start_joints(&self, bodies: &mut [SimBody], joints: &[JointConstraint]) {
        for j in joints {
            if j.accumulated_impulse.abs() <= 1.0e-8 {
                continue;
            }

            let impulse = j.axis * j.accumulated_impulse;
            let (a, b) = get2_mut(bodies, j.a_idx, j.b_idx);

            if a.is_dynamic() {
                a.velocity -= impulse * a.inv_mass;
                a.angular_velocity -= mul_inv_inertia(a, j.r_a.cross(impulse));
                a.is_awake = true;
                a.sleep_timer = 0.0;
            }
            if b.is_dynamic() {
                b.velocity += impulse * b.inv_mass;
                b.angular_velocity += mul_inv_inertia(b, j.r_b.cross(impulse));
                b.is_awake = true;
                b.sleep_timer = 0.0;
            }
        }
    }

    fn solve_joint_velocity_constraints(&self, bodies: &mut [SimBody], joints: &mut [JointConstraint]) {
        for j in joints {
            if j.effective_mass <= 0.0 {
                continue;
            }

            let (a, b) = get2_mut(bodies, j.a_idx, j.b_idx);
            let vel_a = a.velocity + a.angular_velocity.cross(j.r_a);
            let vel_b = b.velocity + b.angular_velocity.cross(j.r_b);
            let rel = (vel_b - vel_a).dot(j.axis);

            let lambda = -j.effective_mass * (rel * (1.0 + j.damping) + j.bias);
            j.accumulated_impulse += lambda;

            let impulse = j.axis * lambda;
            if a.is_dynamic() {
                a.velocity -= impulse * a.inv_mass;
                a.angular_velocity -= mul_inv_inertia(a, j.r_a.cross(impulse));
            }
            if b.is_dynamic() {
                b.velocity += impulse * b.inv_mass;
                b.angular_velocity += mul_inv_inertia(b, j.r_b.cross(impulse));
            }
        }
    }

    fn solve_joint_position_constraints(&self, bodies: &mut [SimBody], joints: &[JointConstraint]) {
        for j in joints {
            let (a, b) = get2_mut(bodies, j.a_idx, j.b_idx);

            let world_a = a.position + j.local_anchor_a;
            let world_b = b.position + j.local_anchor_b;
            let delta = world_b - world_a;
            let len_sq = delta.length_sq();
            if len_sq <= 1.0e-10 {
                continue;
            }

            let len = len_sq.sqrt();
            let axis = delta * (1.0 / len);
            let error = len - j.rest_length;
            if error.abs() <= 1.0e-4 {
                continue;
            }

            let inv_mass_a = a.inv_mass;
            let inv_mass_b = b.inv_mass;
            let inv_mass_sum = inv_mass_a + inv_mass_b;
            if inv_mass_sum <= 0.0 {
                continue;
            }

            let correction = axis * (error * j.stiffness * 0.5);
            if a.is_dynamic() {
                a.position += correction * (inv_mass_a / inv_mass_sum);
            }
            if b.is_dynamic() {
                b.position -= correction * (inv_mass_b / inv_mass_sum);
            }
        }
    }

    fn prepare_manifolds(&self, bodies: &[SimBody], manifolds: &mut [ContactManifold], dt: f32) {
        if dt <= 0.0 {
            return;
        }

        for m in manifolds {
            let cache = self
                .manifold_cache
                .get(&m.key)
                .filter(|c| c.normal.dot(m.normal) > 0.6);

            for p in &mut m.points {
                let a = &bodies[m.a_idx];
                let b = &bodies[m.b_idx];

                p.r_a = p.point - a.position;
                p.r_b = p.point - b.position;

                if let Some(cached) = cache {
                    let mut best_dist = f32::MAX;
                    let mut best_idx = None;
                    for i in 0..cached.point_count {
                        let d = distance_sq(cached.points[i].point, p.point);
                        if d < best_dist {
                            best_dist = d;
                            best_idx = Some(i);
                        }
                    }
                    if let Some(i) = best_idx {
                        if best_dist < 0.04 * 0.04 {
                            p.normal_impulse = cached.points[i].normal_impulse * self.warm_start_scale;
                            p.tangent_impulse = cached.points[i].tangent_impulse * self.warm_start_scale;
                        }
                    }
                }

                let rv = (b.velocity + b.angular_velocity.cross(p.r_b))
                    - (a.velocity + a.angular_velocity.cross(p.r_a));
                let tangent_vec = rv - m.normal * rv.dot(m.normal);
                p.tangent = if tangent_vec.length_sq() <= 1.0e-8 {
                    pick_perpendicular(m.normal)
                } else {
                    tangent_vec.normalized()
                };

                let inv_mass_a = a.inv_mass;
                let inv_mass_b = b.inv_mass;

                let rn_a = p.r_a.cross(m.normal);
                let rn_b = p.r_b.cross(m.normal);
                let ang_a = mul_inv_inertia(a, rn_a).cross(p.r_a).dot(m.normal);
                let ang_b = mul_inv_inertia(b, rn_b).cross(p.r_b).dot(m.normal);
                let normal_denom = inv_mass_a + inv_mass_b + ang_a + ang_b;
                p.normal_mass = if normal_denom > 1.0e-8 {
                    1.0 / normal_denom
                } else {
                    0.0
                };

                let rt_a = p.r_a.cross(p.tangent);
                let rt_b = p.r_b.cross(p.tangent);
                let tang_ang_a = mul_inv_inertia(a, rt_a).cross(p.r_a).dot(p.tangent);
                let tang_ang_b = mul_inv_inertia(b, rt_b).cross(p.r_b).dot(p.tangent);
                let tangent_denom = inv_mass_a + inv_mass_b + tang_ang_a + tang_ang_b;
                p.tangent_mass = if tangent_denom > 1.0e-8 {
                    1.0 / tangent_denom
                } else {
                    0.0
                };

                p.bias = (self.baumgarte / dt) * (p.penetration - self.penetration_slop).max(0.0);
            }
        }
    }

    fn warm_start_manifolds(&self, bodies: &mut [SimBody], manifolds: &[ContactManifold]) {
        for m in manifolds {
            let (a, b) = get2_mut(bodies, m.a_idx, m.b_idx);
            for p in &m.points {
                let impulse = m.normal * p.normal_impulse + p.tangent * p.tangent_impulse;
                if impulse.length_sq() <= 1.0e-12 {
                    continue;
                }

                if a.is_dynamic() {
                    a.velocity -= impulse * a.inv_mass;
                    a.angular_velocity -= mul_inv_inertia(a, p.r_a.cross(impulse));
                    a.is_awake = true;
                    a.sleep_timer = 0.0;
                }
                if b.is_dynamic() {
                    b.velocity += impulse * b.inv_mass;
                    b.angular_velocity += mul_inv_inertia(b, p.r_b.cross(impulse));
                    b.is_awake = true;
                    b.sleep_timer = 0.0;
                }
            }
        }
    }

    fn solve_velocity_constraints(&self, bodies: &mut [SimBody], manifolds: &mut [ContactManifold]) {
        for m in manifolds {
            let (a, b) = get2_mut(bodies, m.a_idx, m.b_idx);
            if a.inv_mass + b.inv_mass <= 0.0 {
                continue;
            }

            for p in &mut m.points {
                let mut rv = (b.velocity + b.angular_velocity.cross(p.r_b))
                    - (a.velocity + a.angular_velocity.cross(p.r_a));

                let vn = rv.dot(m.normal);
                let mut lambda_n = p.normal_mass * (-vn + p.bias);
                let old_n = p.normal_impulse;
                p.normal_impulse = (old_n + lambda_n).max(0.0);
                lambda_n = p.normal_impulse - old_n;

                let impulse_n = m.normal * lambda_n;
                if a.is_dynamic() {
                    a.velocity -= impulse_n * a.inv_mass;
                    a.angular_velocity -= mul_inv_inertia(a, p.r_a.cross(impulse_n));
                }
                if b.is_dynamic() {
                    b.velocity += impulse_n * b.inv_mass;
                    b.angular_velocity += mul_inv_inertia(b, p.r_b.cross(impulse_n));
                }

                rv = (b.velocity + b.angular_velocity.cross(p.r_b))
                    - (a.velocity + a.angular_velocity.cross(p.r_a));

                let vt = rv.dot(p.tangent);
                let mut lambda_t = p.tangent_mass * (-vt);
                let old_t = p.tangent_impulse;
                let mut new_t = old_t + lambda_t;

                let max_static = m.static_friction * p.normal_impulse;
                let max_dynamic = m.dynamic_friction * p.normal_impulse;
                if new_t.abs() > max_static {
                    new_t = new_t.clamp(-max_dynamic, max_dynamic);
                } else {
                    new_t = new_t.clamp(-max_static, max_static);
                }

                lambda_t = new_t - old_t;
                p.tangent_impulse = new_t;

                let impulse_t = p.tangent * lambda_t;
                if a.is_dynamic() {
                    a.velocity -= impulse_t * a.inv_mass;
                    a.angular_velocity -= mul_inv_inertia(a, p.r_a.cross(impulse_t));
                }
                if b.is_dynamic() {
                    b.velocity += impulse_t * b.inv_mass;
                    b.angular_velocity += mul_inv_inertia(b, p.r_b.cross(impulse_t));
                }
            }
        }
    }

    fn solve_position_constraints(&self, bodies: &mut [SimBody], manifolds: &[ContactManifold]) {
        for m in manifolds {
            let a_idx = m.a_idx;
            let b_idx = m.b_idx;

            let fresh = self.build_manifold(bodies, a_idx, b_idx);
            let Some(fresh) = fresh else {
                continue;
            };
            if fresh.points.is_empty() {
                continue;
            }

            let (a, b) = get2_mut(bodies, a_idx, b_idx);

            let inv_mass_a = a.inv_mass;
            let inv_mass_b = b.inv_mass;
            let inv_mass_sum = inv_mass_a + inv_mass_b;
            if inv_mass_sum <= 0.0 {
                continue;
            }

            let scale = 1.0 / fresh.points.len() as f32;
            for p in fresh.points {
                let correction_mag = ((p.penetration - self.penetration_slop).max(0.0) / inv_mass_sum)
                    * self.position_correction
                    * scale;
                if correction_mag <= 0.0 {
                    continue;
                }

                let correction = fresh.normal * correction_mag;
                if a.is_dynamic() {
                    a.position -= correction * inv_mass_a;
                }
                if b.is_dynamic() {
                    b.position += correction * inv_mass_b;
                }
            }
        }
    }

    fn cache_manifolds(&mut self, manifolds: &[ContactManifold]) {
        for m in manifolds {
            let entry = self.manifold_cache.entry(m.key).or_default();
            entry.normal = m.normal;
            entry.point_count = m.points.len().min(4);
            for i in 0..entry.point_count {
                entry.points[i].point = m.points[i].point;
                entry.points[i].normal_impulse = m.points[i].normal_impulse;
                entry.points[i].tangent_impulse = m.points[i].tangent_impulse;
            }
            entry.last_touched_frame = self.frame_counter;
        }
    }

    fn cache_joint_impulses(&mut self, joints: &[JointConstraint]) {
        for j in joints {
            self.joint_impulse_cache.insert(j.id, j.accumulated_impulse);
        }
    }

    fn prune_manifold_cache(&mut self) {
        let frame = self.frame_counter;
        let max_age = self.cache_persistence_frames;
        self.manifold_cache
            .retain(|_, v| frame.wrapping_sub(v.last_touched_frame) <= max_age);
    }

    fn prune_joint_cache(&mut self) {
        if self.joint_impulse_cache.len() > 4096 {
            self.joint_impulse_cache.clear();
        }
    }

    fn update_sleep_states(&self, dt: f32, bodies: &mut [SimBody]) {
        let sleep_linear_speed = 0.04f32;
        let sleep_linear_speed_sq = sleep_linear_speed * sleep_linear_speed;
        let sleep_angular_speed = 0.06f32;
        let sleep_angular_speed_sq = sleep_angular_speed * sleep_angular_speed;
        let sleep_delay = 0.6f32;

        for b in bodies.iter_mut() {
            if !b.is_dynamic() {
                continue;
            }
            if !b.enable_sleep() {
                b.is_awake = true;
                b.sleep_timer = 0.0;
                continue;
            }

            let low_linear = b.velocity.length_sq() < sleep_linear_speed_sq;
            let low_angular = b.angular_velocity.length_sq() < sleep_angular_speed_sq;
            let low_force = b.force.length_sq() < 1.0e-6;
            let low_torque = b.torque.length_sq() < 1.0e-6;

            if low_linear && low_angular && low_force && low_torque {
                b.sleep_timer += dt;
                if b.sleep_timer >= sleep_delay {
                    b.is_awake = false;
                    b.velocity = Vec3::default();
                    b.angular_velocity = Vec3::default();
                }
            } else {
                b.is_awake = true;
                b.sleep_timer = 0.0;
            }
        }
    }

    fn build_manifold(&self, bodies: &[SimBody], a_idx: usize, b_idx: usize) -> Option<ContactManifold> {
        let a = &bodies[a_idx];
        let b = &bodies[b_idx];

        let obb_a = self.compute_obb(a, a.position, a.orientation);
        let obb_b = self.compute_obb(b, b.position, b.orientation);

        let t_world = obb_b.center - obb_a.center;
        let t = [
            t_world.dot(obb_a.axis[0]),
            t_world.dot(obb_a.axis[1]),
            t_world.dot(obb_a.axis[2]),
        ];

        let mut r = [[0.0f32; 3]; 3];
        let mut abs_r = [[0.0f32; 3]; 3];
        const EPS: f32 = 1.0e-5;
        for i in 0..3 {
            for j in 0..3 {
                r[i][j] = obb_a.axis[i].dot(obb_b.axis[j]);
                abs_r[i][j] = r[i][j].abs() + EPS;
            }
        }

        let a_half = [obb_a.half_extents.x, obb_a.half_extents.y, obb_a.half_extents.z];
        let b_half = [obb_b.half_extents.x, obb_b.half_extents.y, obb_b.half_extents.z];

        let mut min_overlap = f32::MAX;
        let mut best_normal = Vec3 { x: 0.0, y: 1.0, z: 0.0 };
        let mut has_axis = false;

        let mut test_axis = |dist: f32, ra: f32, rb: f32, axis: Vec3, axis_sign: f32| -> bool {
            let overlap = ra + rb - dist;
            if overlap < 0.0 {
                return false;
            }
            if axis.length_sq() <= 1.0e-10 {
                return true;
            }

            let mut n = axis.normalized();
            if axis_sign < 0.0 {
                n = n * -1.0;
            }
            if overlap < min_overlap {
                min_overlap = overlap;
                best_normal = n;
                has_axis = true;
            }
            true
        };

        for i in 0..3 {
            let ra = a_half[i];
            let rb = b_half[0] * abs_r[i][0] + b_half[1] * abs_r[i][1] + b_half[2] * abs_r[i][2];
            let dist = t[i].abs();
            let sign = if t[i] >= 0.0 { 1.0 } else { -1.0 };
            if !test_axis(dist, ra, rb, obb_a.axis[i], sign) {
                return None;
            }
        }

        for j in 0..3 {
            let t_b = t[0] * r[0][j] + t[1] * r[1][j] + t[2] * r[2][j];
            let ra = a_half[0] * abs_r[0][j] + a_half[1] * abs_r[1][j] + a_half[2] * abs_r[2][j];
            let rb = b_half[j];
            let dist = t_b.abs();
            let sign = if t_b >= 0.0 { 1.0 } else { -1.0 };
            if !test_axis(dist, ra, rb, obb_b.axis[j], sign) {
                return None;
            }
        }

        for i in 0..3 {
            for j in 0..3 {
                let axis = obb_a.axis[i].cross(obb_b.axis[j]);
                if axis.length_sq() <= 1.0e-10 {
                    continue;
                }

                let i1 = (i + 1) % 3;
                let i2 = (i + 2) % 3;
                let j1 = (j + 1) % 3;
                let j2 = (j + 2) % 3;

                let ra = a_half[i1] * abs_r[i2][j] + a_half[i2] * abs_r[i1][j];
                let rb = b_half[j1] * abs_r[i][j2] + b_half[j2] * abs_r[i][j1];
                let dist = (t[i2] * r[i1][j] - t[i1] * r[i2][j]).abs();
                let sign = if axis.dot(t_world) >= 0.0 { 1.0 } else { -1.0 };

                if !test_axis(dist, ra, rb, axis, sign) {
                    return None;
                }
            }
        }

        if !has_axis {
            best_normal = t_world.normalized();
            if best_normal.length_sq() <= 1.0e-8 {
                best_normal = Vec3 { x: 0.0, y: 1.0, z: 0.0 };
            }
            min_overlap = 0.0;
        }

        let mut candidates = Vec::with_capacity(16);
        let verts_a = compute_obb_vertices(obb_a);
        let verts_b = compute_obb_vertices(obb_b);

        for v in verts_a {
            if point_inside_obb(v, obb_b) {
                candidates.push(v);
            }
        }
        for v in verts_b {
            if point_inside_obb(v, obb_a) {
                candidates.push(v);
            }
        }

        let support = |box_: Obb, dir: Vec3| -> Vec3 {
            let mut p = box_.center;
            let d0 = dir.dot(box_.axis[0]);
            let d1 = dir.dot(box_.axis[1]);
            let d2 = dir.dot(box_.axis[2]);
            p += box_.axis[0] * if d0 >= 0.0 { box_.half_extents.x } else { -box_.half_extents.x };
            p += box_.axis[1] * if d1 >= 0.0 { box_.half_extents.y } else { -box_.half_extents.y };
            p += box_.axis[2] * if d2 >= 0.0 { box_.half_extents.z } else { -box_.half_extents.z };
            p
        };

        candidates.push((support(obb_a, best_normal) + support(obb_b, best_normal * -1.0)) * 0.5);

        let mut unique = Vec::with_capacity(candidates.len());
        for c in candidates {
            let is_duplicate = unique.iter().any(|u| distance_sq(*u, c) < 1.0e-5);
            if !is_duplicate {
                unique.push(c);
            }
        }

        if unique.is_empty() {
            return None;
        }

        let mut selected = Vec::new();
        if unique.len() <= 4 {
            selected = unique;
        } else {
            let mut centroid = Vec3::default();
            for p in &unique {
                centroid += *p;
            }
            centroid = centroid * (1.0 / unique.len() as f32);

            let t1 = pick_perpendicular(best_normal);
            let t2 = best_normal.cross(t1).normalized();

            pick_extreme(&unique, &mut selected, centroid, t1, true);
            pick_extreme(&unique, &mut selected, centroid, t1, false);
            pick_extreme(&unique, &mut selected, centroid, t2, true);
            pick_extreme(&unique, &mut selected, centroid, t2, false);

            while selected.len() < 4 {
                let mut best_d = -1.0;
                let mut best_idx = None;

                for (idx, candidate) in unique.iter().enumerate() {
                    let dup = selected.iter().any(|s| distance_sq(*s, *candidate) < 1.0e-5);
                    if dup {
                        continue;
                    }

                    let mut min_d = f32::MAX;
                    for s in &selected {
                        min_d = min_d.min(distance_sq(*candidate, *s));
                    }
                    if selected.is_empty() {
                        min_d = distance_sq(*candidate, centroid);
                    }

                    if min_d > best_d {
                        best_d = min_d;
                        best_idx = Some(idx);
                    }
                }

                let Some(idx) = best_idx else {
                    break;
                };
                selected.push(unique[idx]);
            }
        }

        let mut points = Vec::with_capacity(selected.len().min(4));
        for p in selected.into_iter().take(4) {
            points.push(ManifoldPoint {
                point: p,
                penetration: min_overlap.max(0.0),
                ..Default::default()
            });
        }
        if points.is_empty() {
            return None;
        }

        let relative_velocity = b.velocity - a.velocity;
        let _restitution = if relative_velocity.dot(best_normal).abs() < self.restitution_threshold {
            0.0
        } else {
            a.restitution.min(b.restitution)
        };

        Some(ContactManifold {
            a_idx,
            b_idx,
            key: make_pair_key(a.id, b.id),
            normal: best_normal,
            static_friction: (a.static_friction.max(0.0) * b.static_friction.max(0.0)).sqrt(),
            dynamic_friction: (a.dynamic_friction.max(0.0) * b.dynamic_friction.max(0.0)).sqrt(),
            points,
        })
    }

    fn compute_obb(&self, body: &SimBody, at_position: Vec3, at_orientation: Quat) -> Obb {
        let q = at_orientation.normalized();

        let scaled_min = Vec3 {
            x: body.local_bounds_min.x * body.scale.x,
            y: body.local_bounds_min.y * body.scale.y,
            z: body.local_bounds_min.z * body.scale.z,
        };
        let scaled_max = Vec3 {
            x: body.local_bounds_max.x * body.scale.x,
            y: body.local_bounds_max.y * body.scale.y,
            z: body.local_bounds_max.z * body.scale.z,
        };

        let local_center = Vec3 {
            x: 0.5 * (scaled_min.x + scaled_max.x),
            y: 0.5 * (scaled_min.y + scaled_max.y),
            z: 0.5 * (scaled_min.z + scaled_max.z),
        };

        let half_extents = Vec3 {
            x: 0.5 * (scaled_max.x - scaled_min.x).abs(),
            y: 0.5 * (scaled_max.y - scaled_min.y).abs(),
            z: 0.5 * (scaled_max.z - scaled_min.z).abs(),
        };

        let axis = [
            q.rotate(Vec3 { x: 1.0, y: 0.0, z: 0.0 }).normalized(),
            q.rotate(Vec3 { x: 0.0, y: 1.0, z: 0.0 }).normalized(),
            q.rotate(Vec3 { x: 0.0, y: 0.0, z: 1.0 }).normalized(),
        ];

        Obb {
            center: at_position + q.rotate(local_center),
            axis,
            half_extents,
        }
    }

    fn compute_bounding_sphere(
        &self,
        body: &SimBody,
        at_position: Vec3,
        at_orientation: Quat,
        motion_padding: f32,
    ) -> BoundingSphere {
        let obb = self.compute_obb(body, at_position, at_orientation);
        BoundingSphere {
            center: obb.center,
            radius: obb.half_extents.length_sq().sqrt() + motion_padding.max(0.0),
        }
    }

    fn raycast(&self, origin: Vec3, direction: Vec3, max_dist: f32, bodies: &[SimBody]) -> RexRaycastHit {
        if max_dist <= 0.0 {
            return RexRaycastHit::default();
        }

        let dir = direction.normalized();
        if dir.length_sq() <= 1.0e-8 {
            return RexRaycastHit::default();
        }

        let mut closest = max_dist;
        let mut out = RexRaycastHit::default();

        for body in bodies {
            let obb = self.compute_obb(body, body.position, body.orientation);
            if let Some((t, normal)) = raycast_obb(origin, dir, obb, closest) {
                closest = t;
                out.hit = 1;
                out.body = body.id;
                out.distance = t;
                out.point = (origin + dir * t).into();
                out.normal = normal.into();
            }
        }

        out
    }
}

fn get2_mut<T>(slice: &mut [T], i: usize, j: usize) -> (&mut T, &mut T) {
    assert!(i != j);
    if i < j {
        let (left, right) = slice.split_at_mut(j);
        (&mut left[i], &mut right[0])
    } else {
        let (left, right) = slice.split_at_mut(i);
        (&mut right[0], &mut left[j])
    }
}

fn integrate_orientation(q: Quat, omega: Vec3, dt: f32) -> Quat {
    let omega_q = Quat {
        x: omega.x,
        y: omega.y,
        z: omega.z,
        w: 0.0,
    };
    let dq = omega_q.mul(q);
    Quat {
        x: q.x + dq.x * (0.5 * dt),
        y: q.y + dq.y * (0.5 * dt),
        z: q.z + dq.z * (0.5 * dt),
        w: q.w + dq.w * (0.5 * dt),
    }
    .normalized()
}

fn mul_inv_inertia(body: &SimBody, v: Vec3) -> Vec3 {
    let q = body.orientation.normalized();
    let local = q.conjugate().rotate(v);
    let local_applied = Vec3 {
        x: body.inv_inertia.x * local.x,
        y: body.inv_inertia.y * local.y,
        z: body.inv_inertia.z * local.z,
    };
    q.rotate(local_applied)
}

fn distance_sq(a: Vec3, b: Vec3) -> f32 {
    (a - b).length_sq()
}

fn clamp01(v: f32) -> f32 {
    v.clamp(0.0, 1.0)
}

fn pick_perpendicular(n: Vec3) -> Vec3 {
    let mut base = if n.x.abs() < 0.577 {
        Vec3 { x: 1.0, y: 0.0, z: 0.0 }
    } else {
        Vec3 { x: 0.0, y: 1.0, z: 0.0 }
    };
    let mut t = base.cross(n);
    if t.length_sq() <= 1.0e-8 {
        base = Vec3 { x: 0.0, y: 0.0, z: 1.0 };
        t = base.cross(n);
    }
    t.normalized()
}

fn make_pair_key(a: u64, b: u64) -> u64 {
    let low = a.min(b);
    let high = a.max(b);
    let mut key = low;
    key ^= high
        .wrapping_add(0x9e37_79b9_7f4a_7c15)
        .wrapping_add(key << 6)
        .wrapping_add(key >> 2);
    key
}

fn compute_obb_vertices(obb: Obb) -> [Vec3; 8] {
    let mut verts = [Vec3::default(); 8];
    let mut idx = 0usize;
    for x in [-1.0f32, 1.0] {
        for y in [-1.0f32, 1.0] {
            for z in [-1.0f32, 1.0] {
                verts[idx] = obb.center
                    + obb.axis[0] * (obb.half_extents.x * x)
                    + obb.axis[1] * (obb.half_extents.y * y)
                    + obb.axis[2] * (obb.half_extents.z * z);
                idx += 1;
            }
        }
    }
    verts
}

fn point_inside_obb(p: Vec3, obb: Obb) -> bool {
    let eps = 1.0e-4;
    let d = p - obb.center;
    for i in 0..3 {
        let dist = d.dot(obb.axis[i]);
        let ext = match i {
            0 => obb.half_extents.x,
            1 => obb.half_extents.y,
            _ => obb.half_extents.z,
        };
        if dist > ext + eps || dist < -ext - eps {
            return false;
        }
    }
    true
}

fn pick_extreme(
    unique: &[Vec3],
    selected: &mut Vec<Vec3>,
    centroid: Vec3,
    axis: Vec3,
    max_dir: bool,
) {
    let mut best = if max_dir { -f32::MAX } else { f32::MAX };
    let mut best_idx = None;

    for (i, p) in unique.iter().enumerate() {
        let proj = (*p - centroid).dot(axis);
        if (max_dir && proj > best) || (!max_dir && proj < best) {
            best = proj;
            best_idx = Some(i);
        }
    }

    if let Some(i) = best_idx {
        let candidate = unique[i];
        let dup = selected.iter().any(|s| distance_sq(*s, candidate) < 1.0e-5);
        if !dup {
            selected.push(candidate);
        }
    }
}

fn raycast_obb(origin: Vec3, direction: Vec3, obb: Obb, max_dist: f32) -> Option<(f32, Vec3)> {
    let rel = origin - obb.center;
    let o_local = Vec3 {
        x: rel.dot(obb.axis[0]),
        y: rel.dot(obb.axis[1]),
        z: rel.dot(obb.axis[2]),
    };
    let d_local = Vec3 {
        x: direction.dot(obb.axis[0]),
        y: direction.dot(obb.axis[1]),
        z: direction.dot(obb.axis[2]),
    };

    let mut t_min = 0.0f32;
    let mut t_max = max_dist;
    let mut hit_axis = None;
    let mut hit_sign = 1.0f32;

    for i in 0..3 {
        let o = match i {
            0 => o_local.x,
            1 => o_local.y,
            _ => o_local.z,
        };
        let d = match i {
            0 => d_local.x,
            1 => d_local.y,
            _ => d_local.z,
        };
        let min_b = -match i {
            0 => obb.half_extents.x,
            1 => obb.half_extents.y,
            _ => obb.half_extents.z,
        };
        let max_b = -min_b;

        if d.abs() < 1.0e-7 {
            if o < min_b || o > max_b {
                return None;
            }
            continue;
        }

        let mut t1 = (min_b - o) / d;
        let mut t2 = (max_b - o) / d;
        let mut sign1 = -1.0f32;
        let mut sign2 = 1.0f32;
        if t1 > t2 {
            std::mem::swap(&mut t1, &mut t2);
            std::mem::swap(&mut sign1, &mut sign2);
        }

        if t1 > t_min {
            t_min = t1;
            hit_axis = Some(i);
            hit_sign = sign1;
        }
        t_max = t_max.min(t2);
        if t_min > t_max {
            return None;
        }
    }

    if t_min < 0.0 || t_min > max_dist {
        return None;
    }

    let normal = if let Some(axis) = hit_axis {
        obb.axis[axis] * hit_sign
    } else {
        direction * -1.0
    }
    .normalized();

    Some((t_min, normal))
}

#[no_mangle]
pub extern "C" fn rex_physics_world_create() -> *mut RexPhysicsWorld {
    Box::into_raw(Box::new(RexPhysicsWorld::new()))
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_destroy(world: *mut RexPhysicsWorld) {
    if world.is_null() {
        return;
    }
    drop(Box::from_raw(world));
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_set_gravity(world: *mut RexPhysicsWorld, gravity: RexVec3) {
    if let Some(world) = world.as_mut() {
        world.gravity = gravity.into();
    }
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_set_solver_iterations(
    world: *mut RexPhysicsWorld,
    velocity_iterations: i32,
    position_iterations: i32,
) {
    if let Some(world) = world.as_mut() {
        world.solver_iterations = velocity_iterations.max(1);
        world.position_iterations = position_iterations.max(1);
    }
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_set_max_substeps(world: *mut RexPhysicsWorld, max_sub_steps: i32) {
    if let Some(world) = world.as_mut() {
        world.max_sub_steps = max_sub_steps.clamp(1, 16);
    }
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_step(
    world: *mut RexPhysicsWorld,
    dt: f32,
    bodies: *mut RexPhysicsBody,
    body_count: usize,
    joints: *const RexDistanceJoint,
    joint_count: usize,
) {
    let Some(world) = world.as_mut() else {
        return;
    };

    if bodies.is_null() || body_count == 0 {
        return;
    }

    let body_slice = std::slice::from_raw_parts_mut(bodies, body_count);
    let mut sim_bodies = Vec::with_capacity(body_count);
    for raw in body_slice.iter() {
        sim_bodies.push(SimBody::from_ffi(raw));
    }

    let joint_slice = if joints.is_null() || joint_count == 0 {
        &[]
    } else {
        std::slice::from_raw_parts(joints, joint_count)
    };

    world.step(dt, &mut sim_bodies, joint_slice);

    for (sim, raw) in sim_bodies.iter().zip(body_slice.iter_mut()) {
        sim.write_back(raw);
    }
}

#[no_mangle]
pub unsafe extern "C" fn rex_physics_world_raycast(
    world: *mut RexPhysicsWorld,
    origin: RexVec3,
    direction: RexVec3,
    max_dist: f32,
    bodies: *const RexPhysicsBody,
    body_count: usize,
    out_hit: *mut RexRaycastHit,
) {
    if out_hit.is_null() {
        return;
    }

    let Some(world) = world.as_mut() else {
        *out_hit = RexRaycastHit::default();
        return;
    };

    if bodies.is_null() || body_count == 0 {
        *out_hit = RexRaycastHit::default();
        return;
    }

    let body_slice = std::slice::from_raw_parts(bodies, body_count);
    let mut sim_bodies = Vec::with_capacity(body_count);
    for raw in body_slice {
        sim_bodies.push(SimBody::from_ffi(raw));
    }

    *out_hit = world.raycast(origin.into(), direction.into(), max_dist, &sim_bodies);
}
