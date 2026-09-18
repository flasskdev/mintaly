#pragma once

namespace systems::native_preview_detail {
inline constexpr const char* bootstrap = R"MINTALY_JS(
(function () {
    'use strict';
    var root = $.GetContextPanel();
    if (!root || !root.IsValid()) return;
    var data = root.Data();
    if (data.mintalyNativePreview && data.mintalyNativePreview.version === 5) return;
    var state = { version: 5, panel: null, generation: '', touched: 0, args: null };
    data.mintalyNativePreview = state;
    function logMsg(msg) {
        try {
            $.Msg('[mintaly preview] ' + msg);
            if (typeof GameInterfaceAPI !== 'undefined' && typeof GameInterfaceAPI.ConsoleCommand === 'function') {
                GameInterfaceAPI.ConsoleCommand('echoln "[mintaly-js] ' + String(msg).replace(/"/g, "'") + '"');
            }
        } catch (_) {}
    }
    function destroy() {
        if (state.panel && state.panel.IsValid()) state.panel.DeleteAsync(0);
        state.panel = null;
        state.generation = '';
    }
    function optional(panel, method, args) {
        try {
            if (panel && typeof panel[method] === 'function') return panel[method].apply(panel, args);
        } catch (e) {
            logMsg('call ' + method + ' failed: ' + e);
        }
        return undefined;
    }
    function faux(def, paint) {
        try {
            if (typeof InventoryAPI !== 'undefined' && typeof InventoryAPI.GetFauxItemIDFromDefAndPaintIndex === 'function') {
                var id = InventoryAPI.GetFauxItemIDFromDefAndPaintIndex(def, paint);
                if (id && InventoryAPI.IsValidItemID(id)) return id;
                if (paint !== 0) {
                    var id0 = InventoryAPI.GetFauxItemIDFromDefAndPaintIndex(def, 0);
                    if (id0 && InventoryAPI.IsValidItemID(id0)) return id0;
                }
            }
        } catch (e) {
            logMsg('faux error for def=' + def + ' paint=' + paint + ': ' + e);
        }
        return '';
    }
    function cameraFor(id) {
        var name = '';
        try { if (typeof InventoryAPI !== 'undefined') name = InventoryAPI.GetItemDefinitionName(id); } catch (_) {}
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
        var category = '';
        try { if (typeof InventoryAPI !== 'undefined') category = InventoryAPI.GetLoadoutCategory(id); } catch (_) {}
        return 'cam_' + (category === 'secondary' ? '0' : category === 'smg' ? '2' : '3');
    }
    function applyRotation() {
        var p = state.panel, a = state.args;
        if (!p || !p.IsValid() || !a) return;
        optional(p, 'SetRotation', [a.yaw, a.pitch, 0]);
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
                    try { def = InventoryAPI.GetItemDefinitionIndexFromDefinitionName('musickit'); } catch (_) {}
                    paint = a.music;
                    active = 4; camera = 'cam_musickit_close';
                } else if (a.kind === 'knife') { active = 8; camera = 'cam_melee'; }
                else if (a.kind === 'gloves') { active = 7; camera = 'cam_gloves'; }
                id = faux(def, paint);
                if (a.kind === 'weapon') camera = cameraFor(id);
            }

            var panelType = agent ? 'MapPlayerPreviewPanel' : 'MapItemPreviewPanel';
            var p = $.CreatePanel(panelType, root, 'MintalyNativePreview', {
                'require-composition-layer': true,
                'composition-layer-texture-name': a.texture,
                'pin-fov': 'vertical',
                'transparent-background': true,
                'disable-depth-of-field': true,
                map: 'ui/acknowledge_item',
                camera: camera,
                initial_entity: 'item',
                player: true,
                playername: 'vanity_character',
                animgraphcharactermode: 'inventory-inspect',
                animgraphturns: false,
                mouse_rotate: false,
                rotation_limit_x: 360,
                rotation_limit_y: 90,
                auto_rotate_x: 0,
                auto_rotate_y: 0,
                auto_recenter: false
            });
            if (!p) {
                logMsg('panel creation returned null for type=' + panelType);
                throw new Error('panel creation failed');
            }
            state.panel = p;
            p.hittest = false;
            p.hittestchildren = false;
            p.style.width = a.width + 'px';
            p.style.height = a.height + 'px';
            p.style.opacity = '0.01';
            p.style.washColor = '#00000000';
            p.style.position = '0px 0px 0px';
            p.style.zIndex = -99999;
            p.style.visibility = 'visible';
            try { p.style['require-composition-layer'] = 'true'; } catch (_) {}
            try { p.style['composition-layer-texture-name'] = a.texture; } catch (_) {}
            try { p.style.requireCompositionLayer = 'true'; } catch (_) {}
            try { p.style.compositionLayerTextureName = a.texture; } catch (_) {}
            logMsg('created panel: ' + panelType + ' texture=' + a.texture + ' id=' + id);

            if (typeof InventoryAPI !== 'undefined' && typeof InventoryAPI.PrecacheCustomMaterials === 'function' && id) {
                try { InventoryAPI.PrecacheCustomMaterials(id); } catch (_) {}
            }
            function configure() {
                if (!p.IsValid() || state.panel !== p) return;
                try {
                    if (agent) {
                        optional(p, 'SetActiveCharacter', [active]);
                        if (id) optional(p, 'SetPlayerCharacterItemID', [id]);
                        if (a.model) optional(p, 'SetPlayerModel', [a.model]);
                    } else {
                        optional(p, 'SetActiveItem', [active]);
                        if (id) optional(p, 'SetItemItemId', [id, '']);
                        else if (a.model) optional(p, 'SetItemModel', [a.model]);
                    }
                    optional(p, 'TransitionToCamera', [camera, 0]);
                    optional(p, 'SetHideStaticGeometry', [true]);
                    optional(p, 'SetHideParticles', [true]);
                    optional(p, 'SetTransparentBackground', [true]);
                    optional(p, 'SetBackgroundColor', [0, 0, 0, 0]);
                    for (var i = 0; i <= 8; ++i) {
                        optional(p, 'FireEntityInput', ['light_item' + i, i === active ? 'Enable' : 'Disable']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'Disable']);
                    }
                    applyRotation();
                    logMsg('configured successfully for texture=' + a.texture);
                } catch (ce) {
                    logMsg('configure error: ' + ce);
                }
            }
            configure();
            state.generation = a.texture;
            [0.15, 0.5, 1.0].forEach(function (delay) {
                $.Schedule(delay, function () {
                    if (!p.IsValid() || state.panel !== p) return;
                    configure();
                });
            });
        } catch (e) {
            destroy();
            logMsg('update exception: ' + e + (e && e.stack ? ' @ ' + e.stack : ''));
        }
    };
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
