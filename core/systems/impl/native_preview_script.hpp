#pragma once

namespace systems::native_preview_detail {
inline constexpr const char* bootstrap = R"MINTALY_JS(
(function () {
    'use strict';
    // Loaded in the existing, validated HUD/main-menu context. Never touch the
    // game's own inspect panels or mutate InventoryAPI / LoadoutAPI state.
    var root = $.GetContextPanel();
    if (!root || !root.IsValid()) return;
    var data = root.Data();
    if (data.mintalyNativePreview && data.mintalyNativePreview.version === 1) return;
    var state = { version: 1, panel: null, generation: '', touched: 0, args: null };
    data.mintalyNativePreview = state;
    function destroy() {
        if (state.panel && state.panel.IsValid()) state.panel.DeleteAsync(0);
        state.panel = null;
        state.generation = '';
    }
    function required(panel, method, args) {
        if (typeof panel[method] !== 'function') throw new Error('missing ' + method);
        return panel[method].apply(panel, args);
    }
    function optional(panel, method, args) {
        if (typeof panel[method] === 'function') panel[method].apply(panel, args);
    }
    function faux(def, paint) {
        var id = InventoryAPI.GetFauxItemIDFromDefAndPaintIndex(def, paint);
        if (!id || !InventoryAPI.IsValidItemID(id)) throw new Error('invalid catalogue item');
        return id;
    }
    function cameraFor(id) {
        var name = InventoryAPI.GetItemDefinitionName(id);
        var cameras = {
            weapon_awp: '7', weapon_aug: '3', weapon_sg556: '4', weapon_ssg08: '6',
            weapon_ak47: '4', weapon_m4a1_silencer: '6', weapon_famas: '4',
            weapon_g3sg1: '5', weapon_galilar: '3', weapon_m4a1: '4', weapon_scar20: '5',
            weapon_mp5sd: '3', weapon_xm1014: '4', weapon_m249: '6', weapon_ump45: '3',
            weapon_bizon: '3', weapon_mag7: '3', weapon_nova: '5', weapon_sawedoff: '3',
            weapon_negev: '5', weapon_usp_silencer: '2', weapon_elite: '2',
            weapon_tec9: '2', weapon_revolver: '2', weapon_c4: '3', weapon_taser: '0'
        };
        if (cameras[name]) return 'cam_' + cameras[name];
        var category = InventoryAPI.GetLoadoutCategory(id);
        return 'cam_' + (category === 'secondary' ? '0' : category === 'smg' ? '2' : '3');
    }
    function applyRotation() {
        var p = state.panel, a = state.args;
        if (!p || !p.IsValid() || !a) return;
        if (typeof p.SetRotation === 'function') p.SetRotation(a.yaw, a.pitch, 0);
        // Native player panels vary between game builds. Do not call guessed
        // vtables or write a model transform through offsets if this is absent.
    }
    state.update = function (a) {
        state.touched = Date.now();
        if (!a.visible) { destroy(); return; }
        state.args = a;
        if (state.generation === a.texture && state.panel && state.panel.IsValid()) {
            applyRotation();
            return;
        }
        destroy();
        try {
            var agent = a.kind === 'agent', id = '', active = 0, camera = 'cam_default';
            if (agent) {
                if (a.def > 0) id = faux(a.def, 0);
                active = 5; camera = 'cam_char_inspect_wide';
            } else {
                var def = a.def, paint = a.paint;
                if (a.kind === 'music') {
                    def = InventoryAPI.GetItemDefinitionIndexFromDefinitionName('musickit');
                    paint = a.music;
                    active = 4; camera = 'cam_musickit_close';
                } else if (a.kind === 'knife') { active = 8; camera = 'cam_melee'; }
                else if (a.kind === 'gloves') { active = 7; camera = 'cam_gloves'; }
                id = faux(def, paint);
                if (a.kind === 'weapon') camera = cameraFor(id);
            }
            var p = $.CreatePanel(agent ? 'MapPlayerPreviewPanel' : 'MapItemPreviewPanel',
                root, 'MintalyNativePreview', {
                    'require-composition-layer': 'true',
                    'composition-layer-texture-name': a.texture,
                    'pin-fov': 'vertical',
                    'transparent-background': 'true',
                    'disable-depth-of-field': 'true',
                    map: 'ui/acknowledge_item', camera: camera,
                    initial_entity: 'item', player: 'true',
                    playername: 'vanity_character', animgraphcharactermode: 'inventory-inspect',
                    animgraphturns: 'false', mouse_rotate: 'false',
                    rotation_limit_x: '360', rotation_limit_y: '90',
                    auto_rotate_x: '0', auto_rotate_y: '0', auto_recenter: 'false'
                });
            if (!p) throw new Error('panel creation failed');
            state.panel = p;
            // Nonzero opacity and on-screen bounds avoid visibility culling.
            // The composition texture itself is sampled by the C++ menu.
            p.hittest = false;
            p.hittestchildren = false;
            p.style.width = a.width + 'px';
            p.style.height = a.height + 'px';
            p.style.position = '0px 0px 0px';
            p.style.opacity = '0.01';
            p.style.visibility = 'visible';
            p.style.zIndex = '-9999';
            if (typeof InventoryAPI.PrecacheCustomMaterials === 'function' && id)
                InventoryAPI.PrecacheCustomMaterials(id);
            function configure() {
                if (!p.IsValid() || state.panel !== p) return;
                if (agent) {
                    required(p, 'SetActiveCharacter', [active]);
                    if (id) required(p, 'SetPlayerCharacterItemID', [id]);
                    required(p, 'SetPlayerModel', [a.model]);
                } else {
                    required(p, 'SetActiveItem', [active]);
                    required(p, 'SetItemItemId', [id, '']);
                }
                required(p, 'TransitionToCamera', [camera, 0]);
                optional(p, 'SetHideStaticGeometry', [true]);
                optional(p, 'SetHideParticles', [true]);
                optional(p, 'SetTransparentBackground', [true]);
                optional(p, 'SetBackgroundColor', [0, 0, 0, 0]);
                // Native inspect map has separate lighting per display slot.
                for (var i = 0; i <= 8; ++i) {
                    optional(p, 'FireEntityInput', ['light_item' + i, i === active ? 'Enable' : 'Disable']);
                    optional(p, 'FireEntityInput', ['light_item_new' + i, 'Disable']);
                }
                applyRotation();
            }
            configure();
            state.generation = a.texture;
            // Map loading is asynchronous. Bounded retries also cover slow disks.
            [0.15, 0.5, 1.0].forEach(function (delay) {
                $.Schedule(delay, function () {
                    if (!root.IsValid() || state.panel !== p || !p.IsValid()) return;
                    try { configure(); } catch (e) { destroy(); $.Msg('[mintaly preview] ' + e); }
                });
            });
        } catch (e) {
            destroy();
            $.Msg('[mintaly preview] ' + e);
        }
    };
    // Auto-cleanup even if the native plugin is unloaded without another UI tick.
    function watchdog() {
        if (!root.IsValid()) return;
        if (Date.now() - state.touched > 1500) destroy();
        if (state.panel) $.Schedule(0.5, watchdog);
        else state.watching = false;
    }
    state.submit = function (a) {
        state.update(a);
        if (state.panel && !state.watching) { state.watching = true; $.Schedule(0.5, watchdog); }
    };
})();
)MINTALY_JS";
}
