#pragma once

namespace systems::native_preview_detail {
inline constexpr char agent_entity_name_prefix[] = "dynamic_player";
inline constexpr const char* bootstrap = R"MINTALY_JS(
(function () {
    'use strict';
    var context = $.GetContextPanel();
    if (!context || !context.IsValid()) return;
    var root = context;
    var chain = [];
    for (var depth = 0; depth < 16 && root && root.IsValid(); ++depth) {
        chain.push(root);
        var parent = null;
        try { parent = root.GetParent(); } catch (_) {}
        if (!parent || !parent.IsValid()) break;
        root = parent;
    }
    // Remove stale previews so the updated panel settings take effect immediately.
    for (var ci = 0; ci < chain.length; ++ci) {
        try {
            var old = chain[ci].Data().mintalyNativePreview;
            if (old && old.version !== 23) {
                if (old.anchor && old.anchor.IsValid()) old.anchor.style.visibility = 'hidden';
                if (old.panel && old.panel.IsValid()) old.panel.DeleteAsync(0);
                if (old.background && old.background.IsValid()) old.background.DeleteAsync(0);
                if (old.anchor && old.anchor.IsValid()) old.anchor.DeleteAsync(0);
                old.panel = null; old.background = null; old.anchor = null;
            }
        } catch (_) {}
    }
    // Always run from the live HUD context. Preview panels get a screen-sized
    // anchor below because this root can report a zero layout size in-game.
    var host = root;
    var data = root.Data();
    if (data.mintalyNativePreview && data.mintalyNativePreview.version === 23) return;
    var state = { version: 23, panel: null, background: null, anchor: null, generation: '', touched: 0, args: null };
    data.mintalyNativePreview = state;
    function logMsg(msg) {
        try {
            $.Msg('[mintaly preview] ' + msg);
            if (typeof GameInterfaceAPI !== 'undefined' && typeof GameInterfaceAPI.ConsoleCommand === 'function') {
                GameInterfaceAPI.ConsoleCommand('echoln "[mintaly-js] ' + String(msg).replace(/"/g, "'") + '"');
            }
        } catch (_) {}
    }
    function inspectTree() {
        var queue = [{ panel: root, depth: 0 }], sized = [], zeroSize = [], visited = 0;
        for (var cursor = 0; cursor < queue.length && cursor < 2048; ++cursor) {
            var item = queue[cursor], panel = item.panel;
            if (!panel || !panel.IsValid() || item.depth > 32) continue;
            ++visited;
            var w = Number(panel.actuallayoutwidth) || Number(panel.contentwidth) || 0;
            var h = Number(panel.actuallayoutheight) || Number(panel.contentheight) || 0;
            var row = (panel.id || panel.paneltype || 'Panel') + ':' + Math.round(w) + 'x' + Math.round(h);
            if (w >= 64 && h >= 64 && sized.length < 12) sized.push(row);
            else if (zeroSize.length < 6) zeroSize.push(row);
            try {
                var children = panel.Children ? panel.Children() : [];
                for (var i = 0; children && i < children.length && queue.length < 2048; ++i)
                    queue.push({ panel: children[i], depth: item.depth + 1 });
            } catch (_) {}
        }
        logMsg('context root=' + (root.id || root.paneltype || '<unknown>') +
            ' nodes=' + visited + ' sized=' + sized.join(',') + ' sample=' + zeroSize.join(','));
    }
    inspectTree();
    function destroy() {
        // DeleteAsync is deferred by Panorama. Collapse the full-screen host
        // first so a closed menu can never leave its old preview on screen.
        if (state.anchor && state.anchor.IsValid()) state.anchor.style.visibility = 'hidden';
        if (state.panel && state.panel.IsValid()) state.panel.DeleteAsync(0);
        if (state.background && state.background.IsValid()) state.background.DeleteAsync(0);
        if (state.anchor && state.anchor.IsValid()) state.anchor.DeleteAsync(0);
        state.panel = null;
        state.background = null;
        state.anchor = null;
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
    function pistolForTeam(team) {
        var teamName = team === 2 ? 't' : 'ct';
        try {
            if (typeof LoadoutAPI !== 'undefined' && typeof LoadoutAPI.GetLoadoutSlotNames === 'function') {
                var slots = JSON.parse(LoadoutAPI.GetLoadoutSlotNames(false) || '[]');
                for (var i = 0; i < slots.length; ++i) {
                    if (String(slots[i]).indexOf('secondary') !== 0) continue;
                    var equipped = LoadoutAPI.GetItemID(teamName, slots[i]);
                    if (equipped && InventoryAPI.IsValidItemID(equipped) &&
                        InventoryAPI.GetLoadoutCategory(equipped) === 'secondary')
                        return equipped;
                }
            }
        } catch (e) {
            logMsg('equipped pistol lookup failed: ' + e);
        }
        // USP-S for CT and Glock-18 for T keep the preview armed even before a
        // loadout has finished syncing in the main menu.
        return faux(team === 2 ? 4 : 61, 0);
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
        // The preview panel already orients its actor towards the camera. Pushing
        // an explicit zero rotation would replace that facing with the raw spawn
        // orientation, which shows the agent from the side. Only rotate once the
        // user actually drags the preview.
        if (a.kind === 'agent' && !Number(a.yaw) && !Number(a.pitch)) return;
        // Panorama's angle vector is pitch, yaw, roll. Keep horizontal drag on
        // yaw so it turns the character around instead of pitching it forward.
        optional(p, 'SetRotation', [a.pitch, a.yaw, 0]);
        optional(p, 'SetSceneAngles', [a.pitch, a.yaw, 0]);
    }
    function applyLayout() {
        var p = state.panel, bg = state.background, a = state.args;
        if (!p || !p.IsValid() || !a) return;
        var x = a.x, y = a.y, w = a.width, h = a.height;
        var rgb = Number(a.background_rgb || 0x0c0d10) & 0xffffff;
        var color = '#' + ('000000' + rgb.toString(16)).slice(-6);
        if (bg && bg.IsValid()) {
            bg.style.position = x + 'px ' + y + 'px 0px';
            bg.style.width = w + 'px';
            bg.style.height = h + 'px';
            bg.style.visibility = 'visible';
            bg.style.zIndex = 99998;
            bg.style.backgroundImage = 'none';
            bg.style.backgroundColor = color;
            bg.style.opacity = '1.0';
        }
        p.style.position = x + 'px ' + y + 'px 0px';
        p.style.width = w + 'px';
        p.style.height = h + 'px';
        p.style.visibility = 'visible';
        p.style.zIndex = 99999;
        p.style.backgroundColor = 'transparent';
        p.style.backgroundImage = 'none';
        p.style.washColor = 'rgba(0,0,0,0)';
    }
    state.update = function (a) {
        state.touched = Date.now();
        if (!a.visible) { destroy(); return; }
        state.args = a;
        var generation = [a.kind, a.def, a.paint, a.music, a.team, a.model].join('|');
        if (state.generation === generation && state.panel && state.panel.IsValid()) {
            applyLayout();
            applyRotation();
            return;
        }
        destroy();
        try {
        function getAgentMap() {
            var bg = '';
            try {
                if (typeof GameInterfaceAPI !== 'undefined' && typeof GameInterfaceAPI.GetSettingString === 'function') {
                    bg = GameInterfaceAPI.GetSettingString('ui_inspect_bkgnd_map');
                    if (!bg || bg === 'mainmenu') {
                        bg = GameInterfaceAPI.GetSettingString('ui_mainmenu_bkgnd_movie');
                    }
                }
            } catch (_) {}
            if (bg && bg !== 'mainmenu') {
                return bg + '_vanity';
            }
            return 'warehouse_vanity';
        }
        var agent = a.kind === 'agent', id = '', active = 0, camera = 'cam_default';
        var loadedMap = 'ui/acknowledge_item';
        if (agent) {
            if (a.def > 0) id = faux(a.def, 0);
            active = 5;
            camera = 'cam_char_inspect_wide_intro';
            loadedMap = getAgentMap();
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

            var pistolId = agent ? pistolForTeam(a.team) : '';
            var panelType = agent ? 'MapPlayerPreviewPanel' : 'MapItemPreviewPanel';
            var anchor = $.CreatePanel('Panel', host, 'MintalyNativePreviewAnchor', {});
            if (!anchor) throw new Error('preview anchor creation failed');
            state.anchor = anchor;
            anchor.hittest = false;
            anchor.hittestchildren = false;
            anchor.style.position = '0px 0px 0px';
            anchor.style.width = Math.max(64, Number(a.screen_width) || 1920) + 'px';
            anchor.style.height = Math.max(64, Number(a.screen_height) || 1080) + 'px';
            anchor.style.overflow = 'noclip';
            anchor.style.visibility = 'visible';
            anchor.style.zIndex = 99997;
            var bg = $.CreatePanel('Panel', anchor, 'MintalyNativePreviewBackground', {});
            if (!bg) throw new Error('preview background creation failed');
            state.background = bg;
            bg.hittest = false;
            bg.hittestchildren = false;
            var panelAttributes = {
                'require-composition-layer': 'true',
                'pin-fov': 'vertical',
                'transparent-background': 'true',
                'disable-depth-of-field': true,
                map: loadedMap,
                camera: camera,
                player: 'true',
                initial_entity: 'item',
                playername: 'vanity_character',
                animgraphcharactermode: 'inventory-inspect',
                animgraphturns: 'false',
                sync_spawn_addons: 'true',
                csm_split_plane0_distance_override: '200.0',
                hide_while_waiting_for_composite_materials: 'false',
                mouse_rotate: false,
                rotation_limit_x: 360,
                rotation_limit_y: 90,
                auto_rotate_x: 0,
                auto_rotate_y: 0,
            };
            panelAttributes.class = 'full-width full-height';
            var p = $.CreatePanel(panelType, anchor, 'MintalyNativePreview', panelAttributes);
            if (!p) {
                logMsg('panel creation returned null for type=' + panelType);
                throw new Error('panel creation failed');
            }
            state.panel = p;
            logMsg('panel background api: transparent=' + (typeof p.SetTransparentBackground) +
                ' color=' + (typeof p.SetBackgroundColor));
            p.Data().itemId = id;
            p.Data().active_item_idx = active;
            if (agent) p.Data().weaponItemId = pistolId;
            p.Data().loadedMap = loadedMap;
            p.hittest = false;
            p.hittestchildren = false;
            p.style.opacity = '0.0';
            p.style.transform = 'scale3d(1, 1, 1)';
            p.style.transformOrigin = '50% 50%';
            p.style.transitionProperty = 'opacity';
            p.style.transitionDuration = '0.25s';
            p.style.transitionTimingFunction = 'ease-out';
            p.style.visibility = 'visible';
            p.style.zIndex = 99999;
            applyLayout();
            logMsg('created native preview panel: ' + panelType + ' id=' + id +
                ' host=' + (host.id || '<root>') + ' playername=' +
                (agent ? 'mintaly_agent_preview_actor' : '<item>') +
                ' anchor=' + anchor.style.width + 'x' + anchor.style.height);

            if (typeof InventoryAPI !== 'undefined' && typeof InventoryAPI.PrecacheCustomMaterials === 'function' && id) {
                try { InventoryAPI.PrecacheCustomMaterials(id); } catch (_) {}
            }
            function configure(attempt) {
                if (!p.IsValid() || state.panel !== p) return;
                try {
                    if (agent) {
                        optional(p, 'SetActiveCharacter', [5]);
                        var configuredByGameHelpers = false;
                        try {
                            if (typeof ItemInfo !== 'undefined' &&
                                typeof ItemInfo.GetOrUpdateVanityCharacterSettings === 'function' &&
                                typeof CharacterAnims !== 'undefined' &&
                                typeof CharacterAnims.PlayAnimsOnPanel === 'function') {
                                var characterSettings = ItemInfo.GetOrUpdateVanityCharacterSettings(id);
                                characterSettings.panel = p;
                                characterSettings.team = a.team === 2 ? 't' : 'ct';
                                if (id) characterSettings.charItemId = id;
                                if (pistolId) {
                                    characterSettings.weaponItemId = pistolId;
                                    characterSettings.weaponItemID = pistolId;
                                }
                                if (a.model) characterSettings.modelOverride = a.model;
                                CharacterAnims.PlayAnimsOnPanel(characterSettings);
                                configuredByGameHelpers = true;
                            }
                        } catch (helperError) {
                            logMsg('agent helper setup failed: ' + helperError);
                        }
                        if (!configuredByGameHelpers) {
                            if (id) optional(p, 'SetPlayerCharacterItemID', [id]);
                            if (a.model) optional(p, 'SetPlayerModel', [a.model]);
                        }
                        // Apply the secondary after the vanity helper too: some
                        // client builds ignore weaponItemId in the settings object.
                        if (pistolId) {
                            optional(p, 'SetPlayerWeaponItemID', [pistolId]);
                            optional(p, 'SetPlayerWeaponItemId', [pistolId]);
                            optional(p, 'EquipPlayerWithItem', [pistolId]);
                        }
                    } else {
                        optional(p, 'SetActiveItem', [active]);
                        if (id) optional(p, 'SetItemItemId', [id, '']);
                        else if (a.model) optional(p, 'SetItemModel', [a.model]);
                    }
                    if (agent) {
                        if (attempt === 1) {
                            optional(p, 'TransitionToCamera', ['cam_char_inspect_wide_intro', 0]);
                            $.Schedule(0.25, function () {
                                if (p.IsValid() && state.panel === p) {
                                    optional(p, 'TransitionToCamera', ['cam_char_inspect_wide', 1]);
                                }
                            });
                        } else {
                            optional(p, 'TransitionToCamera', ['cam_char_inspect_wide', 0]);
                        }
                    } else if (attempt === 1) {
                        optional(p, 'TransitionToCamera', [camera, 0.15]);
                    }
                    optional(p, 'SetHideStaticGeometry', [true]);
                    optional(p, 'SetHideParticles', [true]);
                    var rgb = Number(a.background_rgb || 0x0c0d10) & 0xffffff;
                    optional(p, 'SetTransparentBackground', [true]);
                    // Let the menu-colored sibling backplate show through the
                    // native model composition's clear pixels.
                    optional(p, 'SetBackgroundColor', [(rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255, 0]);
                    optional(p, 'SetCSMSplitPlane0DistanceOverride', [200.0]);
                    optional(p, 'SetBarnlightShadowScaleOverride', [1.0]);
                    for (var i = 0; i <= 10; ++i) {
                        optional(p, 'FireEntityInput', ['light_item' + i, (agent || i !== active) ? 'Disable' : 'Enable']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'Disable']);
                    }
                    applyRotation();
                    applyLayout();
                    p.style.opacity = '1.0';
                    var validId = false;
                    try { validId = !!(id && InventoryAPI.IsValidItemID(id)); } catch (_) {}
                    logMsg('configured pass=' + attempt + ' generation=' + generation +
                        ' host=' + (host.id || '<root>') + ' itemValid=' + validId +
                        ' helper=' + (configuredByGameHelpers ? 'yes' : 'fallback') +
                        ' pistol=' + (pistolId ? 'yes' : 'no') +
                        ' camera=' + camera +
                        ' map=' + loadedMap +
                        ' model=' + (a.model || '<from-item>'));
                } catch (ce) {
                    logMsg('configure pass=' + attempt + ' error: ' + ce);
                }
            }
            state.generation = generation;
            $.Schedule(0.15, function () {
                if (!p.IsValid() || state.panel !== p) return;
                configure(1);
            });
            // MapPlayerPreviewPanel loads its composition scene asynchronously.
            // Re-apply the player and background once the scene has settled;
            // its first frame otherwise keeps only the empty background map.
            [0.75, 1.5, 3.0].forEach(function (delay, index) {
                $.Schedule(delay, function () {
                    if (!p.IsValid() || state.panel !== p) return;
                    configure(index + 2);
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
