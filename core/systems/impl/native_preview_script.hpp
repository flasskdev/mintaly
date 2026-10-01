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
    // Reuse the controller found anywhere on this live context chain. The game
    // can change the script panel without changing the UI engine itself.
    var state = null;
    for (var ci = 0; ci < chain.length; ++ci) {
        try {
            var old = chain[ci].Data().mintalyNativePreview;
            if (old && old.version === 30 && !state) {
                state = old;
            } else if (old && old.version === 30 && state && old !== state) {
                try { if (old.anchor && old.anchor.IsValid()) old.anchor.style.visibility = 'collapse'; } catch (_) {}
                try { if (old.panel && old.panel.IsValid()) old.panel.DeleteAsync(0); } catch (_) {}
                try { if (old.background && old.background.IsValid()) old.background.DeleteAsync(0); } catch (_) {}
                try { if (old.anchor && old.anchor.IsValid()) old.anchor.DeleteAsync(0); } catch (_) {}
                try { old.panel = null; old.background = null; old.anchor = null; } catch (_) {}
            } else if (old && old.version !== 30) {
                try { if (old.anchor && old.anchor.IsValid()) old.anchor.style.visibility = 'collapse'; } catch (_) {}
                try { if (old.panel && old.panel.IsValid()) old.panel.DeleteAsync(0); } catch (_) {}
                try { if (old.background && old.background.IsValid()) old.background.DeleteAsync(0); } catch (_) {}
                try { if (old.anchor && old.anchor.IsValid()) old.anchor.DeleteAsync(0); } catch (_) {}
                try { old.panel = null; old.background = null; old.anchor = null; } catch (_) {}
            }
        } catch (_) {}
    }
    // Always run from the live HUD context. Preview panels get a screen-sized
    // anchor below because this root can report a zero layout size in-game.
    var host = root;
    var data = root.Data();
    if (!state) state = data.mintalyNativePreview;
    if (!state || state.version !== 30) {
        state = { version: 30, panel: null, background: null, anchor: null, generation: '',
            touched: 0, args: null, selection: null, panelFamily: '', panelMap: '',
            configure: null, watching: false, panelSerial: 0, layoutKey: '',
            rotationPanel: null, rotationYaw: null, rotationPitch: null, ready: true };
        data.mintalyNativePreview = state;
    }
    // The bridge can execute from a HUD panel or a menu panel depending on the
    // current scene. Publish the same controller along the whole ancestor chain
    // so each request resolves the exact state regardless of its context depth.
    for (var si = 0; si < chain.length; ++si) {
        try { chain[si].Data().mintalyNativePreview = state; } catch (_) {}
    }
    state.panelSerial = Number(state.panelSerial) || 0;
    state.receivedKey = state.receivedKey || '';
    function logMsg(msg) {
        try {
            $.Msg('[mintaly preview] ' + msg);
            if (typeof GameInterfaceAPI !== 'undefined' && typeof GameInterfaceAPI.ConsoleCommand === 'function') {
                var shortMessage = String(msg).replace(/[\r\n"]/g, "'").slice(0, 96);
                GameInterfaceAPI.ConsoleCommand('echoln "[mintaly-js] ' + shortMessage + '"');
            }
        } catch (_) {}
    }
    if (state.initialized) {
        if (!state.versionLogged) {
            logMsg('bridge initialized version=30');
            state.versionLogged = true;
        }
        return;
    }
    state.initialized = true;
    state.versionLogged = true;
    logMsg('bridge initialized version=30');
    function deletePanel(panel) {
        try { if (panel && panel.IsValid()) panel.DeleteAsync(0); } catch (_) {}
    }
    function collapsePanel(panel) {
        if (!panel) return;
        try { if (!panel.IsValid()) return; } catch (_) { return; }
        try { panel.style.visibility = 'collapse'; } catch (_) {}
        deletePanel(panel);
    }
    function destroy() {
        // Collapse the full-screen host immediately; Panorama only accepts
        // "collapse" or "visible" for visibility, and DeleteAsync is deferred.
        collapsePanel(state.anchor);
        deletePanel(state.panel);
        deletePanel(state.background);
        state.panel = null;
        state.background = null;
        state.anchor = null;
        state.generation = '';
        state.selection = null;
        state.panelFamily = '';
        state.panelMap = '';
        state.configure = null;
        state.layoutKey = '';
        state.rotationPanel = null;
        state.rotationYaw = null;
        state.rotationPitch = null;
        hidePreviewAnchors();
    }
    function hidePreviewAnchors() {
        if (!root || !root.IsValid()) return;
        var queue = [root], cursor = 0, visited = 0;
        while (cursor < queue.length && cursor < 4096 && visited < 4096) {
            var panel = queue[cursor++];
            if (!panel || !panel.IsValid()) continue;
            ++visited;
            var panelId = '';
            try { panelId = String(panel.id || ''); } catch (_) {}
            if (panelId.indexOf('MintalyNativePreviewAnchor_') === 0) {
                collapsePanel(panel);
                continue;
            }
            try {
                var children = panel.Children ? panel.Children() : [];
                for (var i = 0; children && i < children.length && queue.length < 4096; ++i)
                    queue.push(children[i]);
            } catch (_) {}
        }
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
                if (id && String(id) !== '0') {
                    return id;
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
    function agentMap() {
        var bg = '';
        try {
            if (typeof GameInterfaceAPI !== 'undefined' && typeof GameInterfaceAPI.GetSettingString === 'function') {
                bg = GameInterfaceAPI.GetSettingString('ui_inspect_bkgnd_map');
                if (!bg || bg === 'mainmenu') bg = GameInterfaceAPI.GetSettingString('ui_mainmenu_bkgnd_movie');
            }
        } catch (_) {}
        return bg && bg !== 'mainmenu' ? bg + '_vanity' : 'warehouse_vanity';
    }
    function makeSelection(a) {
        var agent = a.kind === 'agent', id = '', active = 0, camera = 'cam_default';
        var loadedMap = agentMap();
        var def = a.def, paint = a.paint;
        if (agent) {
            if (a.def > 0) id = faux(a.def, 0);
            active = 5;
            camera = 'cam_char_inspect_wide_intro';
        } else {
            if (a.kind === 'music') {
                try { def = InventoryAPI.GetItemDefinitionIndexFromDefinitionName('musickit'); } catch (_) {}
                paint = a.music;
                active = 4; camera = 'cam_musickit_close';
            } else if (a.kind === 'knife') { active = 8; camera = 'cam_melee'; }
            else if (a.kind === 'gloves') { active = 7; camera = 'cam_gloves'; }
            id = faux(def, paint);
            if (a.kind === 'weapon') camera = cameraFor(id);
        }
        var key = JSON.stringify([a.kind, a.def, a.paint, a.music, a.team, a.model,
            a.wear, a.seed, a.stattrak ? 1 : 0, a.stattrak_count, a.name_tag]);
        var identity = JSON.stringify([a.kind, a.def, a.paint, a.music, a.team, a.model,
            id, active, loadedMap]);
        return { key: key, identity: identity, agent: agent, id: id, active: active, camera: camera,
            loadedMap: loadedMap, family: agent ? 'player' : 'item', pistolId: agent ? pistolForTeam(a.team) : '' };
    }
    function applyRotation() {
        var p = state.panel, a = state.args;
        if (!p || !p.IsValid() || !a) return;
        var yaw = Number(a.yaw) || 0, pitch = Number(a.pitch) || 0;
        if (state.rotationPanel === p && state.rotationYaw === yaw && state.rotationPitch === pitch) return;
        state.rotationPanel = p;
        state.rotationYaw = yaw;
        state.rotationPitch = pitch;
        // Only push explicit rotation when the user is actively dragging the preview.
        // Forcing (0, 0) every tick resets the inspect animation and caps it to the update rate.
        if (!yaw && !pitch) return;
        // Panorama's angle vector is pitch, yaw, roll. Keep horizontal drag on
        // yaw so it turns the character around instead of pitching it forward.
        optional(p, 'SetRotation', [pitch, yaw, 0]);
        optional(p, 'SetSceneAngles', [pitch, yaw, 0]);
    }
    function applyLayout() {
        var p = state.panel, bg = state.background, anchor = state.anchor, a = state.args;
        if (!p || !p.IsValid() || !a) return;
        var x = a.x, y = a.y, w = a.width, h = a.height;
        var rgb = Number(a.background_rgb || 0x0c0d10) & 0xffffff;
        var layoutKey = JSON.stringify([x, y, w, h, a.screen_width, a.screen_height, rgb]);
        if (state.layoutKey === layoutKey) return;
        state.layoutKey = layoutKey;
        var color = '#' + ('000000' + rgb.toString(16)).slice(-6);
        if (anchor && anchor.IsValid()) {
            anchor.style.width = Math.max(64, Number(a.screen_width) || 1920) + 'px';
            anchor.style.height = Math.max(64, Number(a.screen_height) || 1080) + 'px';
            anchor.style.visibility = 'visible';
        }
        var rCard = (rgb >> 16) & 0xff, gCard = (rgb >> 8) & 0xff, bCard = rgb & 0xff;
        var elevatedColor = '#' + ('000000' + (((Math.min(255, rCard + 8) << 16) | (Math.min(255, gCard + 9) << 8) | Math.min(255, bCard + 12))).toString(16)).slice(-6);
        if (bg && bg.IsValid()) {
            bg.style.position = x + 'px ' + y + 'px 0px';
            bg.style.width = w + 'px';
            bg.style.height = h + 'px';
            bg.style.visibility = 'visible';
            bg.style.zIndex = 99998;
            bg.style.backgroundImage = 'none';
            bg.style.backgroundColor = elevatedColor;
            bg.style.borderRadius = '0px';
            bg.style.opacity = '1.0';
        }
        p.style.position = x + 'px ' + y + 'px 0px';
        p.style.width = w + 'px';
        p.style.height = h + 'px';
        p.style.visibility = 'visible';
        p.style.zIndex = 99999;
        p.style.backgroundColor = 'transparent';
        p.style.borderRadius = '0px';
        p.style.backgroundImage = 'none';
        p.style.washColor = 'rgba(0,0,0,0)';
        p.style.brightness = '1.20';
        p.style.contrast = '1.08';
        p.style.saturation = '1.14';
    }
    state.update = function (a) {
        state.touched = Date.now();
        if (!a.visible) {
            if (state.panel) logMsg('closing preview panel after hide request');
            destroy();
            return;
        }
        state.args = a;
        var requestKey = JSON.stringify([a.kind, a.def, a.paint, a.music, a.team, a.model,
            a.wear, a.seed, a.stattrak ? 1 : 0, a.stattrak_count, a.name_tag]);
        if (state.generation === requestKey && state.panel && state.panel.IsValid()) {
            applyLayout();
            applyRotation();
            return;
        }
        var selection = makeSelection(a);
        var previewWear = Number(a.wear);
        if (!isFinite(previewWear)) previewWear = 0.01;
        var sameItem = !selection.agent && state.panel && state.panel.IsValid() && state.configure &&
            state.selection && state.selection.id === selection.id &&
            state.panelFamily === selection.family && state.panelMap === selection.loadedMap;
        if (sameItem) {
            state.args = a;
            state.generation = requestKey;
            applyLayout();
            applyRotation();
            var p = state.panel;
            var panelData = p.Data();
            panelData.previewWear = previewWear;
            panelData.previewSeed = Number(a.seed) || 0;
            panelData.previewStatTrak = !!a.stattrak;
            panelData.previewStatTrakCount = Number(a.stattrak_count) || 0;
            panelData.previewNameTag = a.name_tag || '';
            optional(p, 'SetItemWear', [previewWear]);
            optional(p, 'SetItemSeed', [Number(a.seed) || 0]);
            optional(p, 'SetItemStatTrak', [a.stattrak ? Number(a.stattrak_count) || 0 : -1]);
            optional(p, 'SetItemStatTrakCount', [a.stattrak ? Number(a.stattrak_count) || 0 : 0]);
            if (a.name_tag) optional(p, 'SetItemCustomName', [a.name_tag]);
            return;
        }
        destroy();
        try {
            // DeleteAsync is deferred, so each new preview generation needs
            // unique Panorama IDs to avoid colliding with the hidden old tree.
            state.panelSerial = (Number(state.panelSerial) || 0) + 1;
            var panelSuffix = String(state.panelSerial);
            var anchor = null;
            try { anchor = $.CreatePanel('Panel', host, 'MintalyNativePreviewAnchor_' + panelSuffix, {}); } catch (_) {}
            if (!anchor && context && context !== host) {
                try { anchor = $.CreatePanel('Panel', context, 'MintalyNativePreviewAnchor_' + panelSuffix, {}); } catch (_) {}
            }
            if (!anchor) throw new Error('preview anchor creation failed');
            try { host.style.overflow = 'noclip'; } catch (_) {}
            try { host.style.visibility = 'visible'; } catch (_) {}
            try { context.style.overflow = 'noclip'; } catch (_) {}
            try { context.style.visibility = 'visible'; } catch (_) {}
            for (var ci = 0; ci < chain.length; ++ci) {
                try { chain[ci].style.overflow = 'noclip'; } catch (_) {}
                try { chain[ci].style.visibility = 'visible'; } catch (_) {}
            }
            state.anchor = anchor;
            anchor.hittest = false;
            anchor.hittestchildren = false;
            anchor.style.position = '0px 0px 0px';
            anchor.style.width = Math.max(64, Number(a.screen_width) || 1920) + 'px';
            anchor.style.height = Math.max(64, Number(a.screen_height) || 1080) + 'px';
            anchor.style.overflow = 'noclip';
            anchor.style.visibility = 'visible';
            anchor.style.zIndex = 99997;
            var initialRgb = Number(a.background_rgb || 0x0c0d10) & 0xffffff;
            var rCardInit = (initialRgb >> 16) & 0xff, gCardInit = (initialRgb >> 8) & 0xff, bCardInit = initialRgb & 0xff;
            var initialColor = '#' + ('000000' + (((Math.min(255, rCardInit + 8) << 16) | (Math.min(255, gCardInit + 9) << 8) | Math.min(255, bCardInit + 12))).toString(16)).slice(-6);
            var bg = $.CreatePanel('Panel', anchor, 'MintalyNativePreviewBg_' + panelSuffix, {});
            state.background = bg;
            if (bg) {
                bg.hittest = false;
                bg.hittestchildren = false;
                bg.style.position = a.x + 'px ' + a.y + 'px 0px';
                bg.style.width = a.width + 'px';
                bg.style.height = a.height + 'px';
                bg.style.backgroundColor = initialColor;
                bg.style.borderRadius = '0px';
                bg.style.zIndex = 99998;
                bg.style.visibility = 'visible';
            }
            if (typeof InventoryAPI !== 'undefined' && typeof InventoryAPI.PrecacheCustomMaterials === 'function' && selection.id) {
                try { InventoryAPI.PrecacheCustomMaterials(selection.id); } catch (_) {}
            }
            var panelAttributes = {
                'require-composition-layer': 'true',
                'pin-fov': 'vertical',
                'transparent-background': 'true',
                'disable-depth-of-field': true,
                map: selection.loadedMap,
                camera: selection.camera,
                player: selection.agent ? 'true' : 'false',
                active_item_idx: selection.active,
                initial_entity: selection.agent ? 'vanity_character' : 'item',
                csm_split_plane0_distance_override: '200.0',
                hide_while_waiting_for_composite_materials: 'true',
                mouse_rotate: false,
                rotation_limit_x: 360,
                rotation_limit_y: 90,
                auto_rotate_x: 0,
                auto_rotate_y: 0,
            };
            if (selection.agent) {
                panelAttributes.playername = 'vanity_character';
                panelAttributes.animgraphcharactermode = 'inventory-inspect';
                panelAttributes.animgraphturns = 'false';
                panelAttributes.sync_spawn_addons = 'true';
            }
            panelAttributes.class = 'full-width full-height';
            var p = $.CreatePanel(selection.agent ? 'MapPlayerPreviewPanel' : 'MapItemPreviewPanel',
                anchor, 'MintalyNativePreview_' + panelSuffix, panelAttributes);
            if (!p) {
                logMsg('panel creation returned null family=' + selection.family);
                throw new Error('panel creation failed');
            }
            state.panel = p;
            state.selection = selection;
            state.panelFamily = selection.family;
            state.panelMap = selection.loadedMap;
            var rCol = (initialRgb >> 16) & 0xff, gCol = (initialRgb >> 8) & 0xff, bCol = initialRgb & 0xff;
            p.hittest = false;
            p.hittestchildren = false;
            p.style.opacity = '1.0';
            p.style.transform = 'scale3d(1, 1, 1)';
            p.style.transformOrigin = '50% 50%';
            p.style.borderRadius = '0px';
            p.style.brightness = '1.20';
            p.style.contrast = '1.08';
            p.style.saturation = '1.14';
            optional(p, 'SetTransparentBackground', [true]);
            optional(p, 'SetHideStaticGeometry', [true]);
            optional(p, 'SetHideParticles', [true]);
            optional(p, 'SetBackgroundColor', [rCol, gCol, bCol, 255]);
            p.Data().itemId = selection.id;
            p.Data().active_item_idx = selection.active;
            if (selection.agent) p.Data().weaponItemId = selection.pistolId;
            p.Data().loadedMap = selection.loadedMap;
            if (!selection.agent && selection.id) {
                optional(p, 'SetActiveItem', [selection.active]);
                optional(p, 'SetItemItemId', [selection.id, '']);
            }
            p.hittest = false;
            p.hittestchildren = false;
            p.style.visibility = 'visible';
            p.style.zIndex = 99999;
            applyLayout();
            logMsg('created native preview panel: ' + (selection.agent ? 'MapPlayerPreviewPanel' : 'MapItemPreviewPanel') + ' id=' + selection.id +
                ' host=' + (host.id || '<root>') + ' playername=' +
                (selection.agent ? 'mintaly_agent_preview_actor' : '<item>') +
                ' anchor=' + anchor.style.width + 'x' + anchor.style.height);

            function configure(attempt) {
                if (!p.IsValid() || state.panel !== p) return;
                var current = state.selection, currentArgs = state.args;
                if (!current || !currentArgs) return;
                try {
                    var panelData = p.Data();
                    panelData.itemId = current.id;
                    panelData.active_item_idx = current.active;
                    panelData.loadedMap = current.loadedMap;
                    panelData.previewPaintKit = Number(currentArgs.paint) || 0;
                    var previewWear = Number(currentArgs.wear);
                    if (!isFinite(previewWear)) previewWear = 0.01;
                    panelData.previewWear = previewWear;
                    panelData.previewSeed = Number(currentArgs.seed) || 0;
                    panelData.previewStatTrak = !!currentArgs.stattrak;
                    panelData.previewStatTrakCount = Number(currentArgs.stattrak_count) || 0;
                    panelData.previewNameTag = currentArgs.name_tag || '';
                    if (current.agent) {
                        panelData.weaponItemId = current.pistolId;
                        optional(p, 'SetActiveCharacter', [5]);
                        var configuredByGameHelpers = false;
                        try {
                            if (typeof ItemInfo !== 'undefined' &&
                                typeof ItemInfo.GetOrUpdateVanityCharacterSettings === 'function' &&
                                typeof CharacterAnims !== 'undefined' &&
                                typeof CharacterAnims.PlayAnimsOnPanel === 'function') {
                                var characterSettings = ItemInfo.GetOrUpdateVanityCharacterSettings(current.id);
                                characterSettings.panel = p;
                                characterSettings.team = currentArgs.team === 2 ? 't' : 'ct';
                                if (current.id) characterSettings.charItemId = current.id;
                                if (current.pistolId) {
                                    characterSettings.weaponItemId = current.pistolId;
                                    characterSettings.weaponItemID = current.pistolId;
                                }
                                if (currentArgs.model) characterSettings.modelOverride = currentArgs.model;
                                CharacterAnims.PlayAnimsOnPanel(characterSettings);
                                configuredByGameHelpers = true;
                            }
                        } catch (helperError) {
                            logMsg('agent helper setup failed: ' + helperError);
                        }
                        if (!configuredByGameHelpers && current.id) {
                            optional(p, 'SetPlayerCharacterItemID', [current.id]);
                        }
                        if (currentArgs.model) {
                            optional(p, 'SetPlayerModel', [currentArgs.model]);
                        }
                        // Apply the secondary after the vanity helper too: some
                        // client builds ignore weaponItemId in the settings object.
                        if (current.pistolId) {
                            optional(p, 'SetPlayerWeaponItemID', [current.pistolId]);
                            optional(p, 'SetPlayerWeaponItemId', [current.pistolId]);
                            optional(p, 'EquipPlayerWithItem', [current.pistolId]);
                        }
                    } else {
                        optional(p, 'SetActiveItem', [current.active]);
                        if (current.id && typeof InventoryAPI !== 'undefined' &&
                            typeof InventoryAPI.PrecacheCustomMaterials === 'function') {
                            try { InventoryAPI.PrecacheCustomMaterials(current.id); } catch (_) {}
                        }
                        var itemIdMethod = 'Data.itemId';
                        if (current.id && typeof p.SetItemItemId === 'function') {
                            optional(p, 'SetItemItemId', [current.id, '']);
                            itemIdMethod = 'SetItemItemId';
                        } else if (current.id && typeof p.SetItemItemID === 'function') {
                            optional(p, 'SetItemItemID', [current.id, '']);
                            itemIdMethod = 'SetItemItemID';
                        }
                        else if (currentArgs.model) optional(p, 'SetItemModel', [currentArgs.model]);
                        optional(p, 'SetItemPaintKit', [Number(currentArgs.paint) || 0]);
                        optional(p, 'SetItemPaintKitIndex', [Number(currentArgs.paint) || 0]);
                        // These setters are version-dependent. Call only when
                        // the current Panorama panel exposes them.
                        optional(p, 'SetItemWear', [previewWear]);
                        optional(p, 'SetItemSeed', [Number(currentArgs.seed) || 0]);
                        optional(p, 'SetItemStatTrak', [currentArgs.stattrak ? Number(currentArgs.stattrak_count) || 0 : -1]);
                        optional(p, 'SetItemStatTrakCount', [currentArgs.stattrak ? Number(currentArgs.stattrak_count) || 0 : 0]);
                        if (currentArgs.name_tag) optional(p, 'SetItemCustomName', [currentArgs.name_tag]);
                    }
                    if (current.agent) {
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
                        optional(p, 'TransitionToCamera', [current.camera, 0.15]);
                    }
                    optional(p, 'SetHideStaticGeometry', [true]);
                    optional(p, 'SetHideParticles', [true]);
                    optional(p, 'SetTransparentBackground', [true]);
                    optional(p, 'SetCSMSplitPlane0DistanceOverride', [200.0]);
                    optional(p, 'SetBarnlightShadowScaleOverride', [1.0]);
                    var itemLightNames = ['light_item', 'light_item_new', 'light_weapon', 'light_item_key', 'light_item_fill', 'light_item_rim'];
                    for (var li = 0; li < itemLightNames.length; ++li) {
                        optional(p, 'FireEntityInput', [itemLightNames[li], 'Enable']);
                        optional(p, 'FireEntityInput', [itemLightNames[li], 'SetColor', '255 255 255']);
                        optional(p, 'FireEntityInput', [itemLightNames[li], 'SetLightBrightness', '7.0']);
                        optional(p, 'FireEntityInput', [itemLightNames[li], 'SetBrightness', '7.0']);
                    }
                    for (var i = 0; i <= 10; ++i) {
                        optional(p, 'FireEntityInput', ['light_item' + i, 'Enable']);
                        optional(p, 'FireEntityInput', ['light_item' + i, 'SetColor', '255 255 255']);
                        optional(p, 'FireEntityInput', ['light_item' + i, 'SetLightBrightness', '6.5']);
                        optional(p, 'FireEntityInput', ['light_item' + i, 'SetBrightness', '6.5']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'Enable']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'SetColor', '255 255 255']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'SetLightBrightness', '6.5']);
                        optional(p, 'FireEntityInput', ['light_item_new' + i, 'SetBrightness', '6.5']);
                    }
                    optional(p, 'FireEntityInput', ['acknowledge_particle', 'DestroyImmediately']);
                    optional(p, 'FireEntityInput', ['acknowledge_particle', 'Disable']);
                    optional(p, 'FireEntityInput', ['sun', 'Enable']);
                    optional(p, 'FireEntityInput', ['sun', 'SetLightBrightness', '6.0']);
                    optional(p, 'FireEntityInput', ['sun', 'SetBrightness', '6.0']);
                    optional(p, 'FireEntityInput', ['main_light', 'Enable']);
                    optional(p, 'FireEntityInput', ['main_light', 'SetBrightness', '6.0']);
                    optional(p, 'FireEntityInput', ['main_light', 'SetLightBrightness', '6.0']);
                    if (current.agent) {
                        var charLightNames = ['light_char', 'light_character', 'light_player', 'light_agent', 'light_head', 'light_rim', 'light_fill', 'light_key'];
                        for (var cli = 0; cli < charLightNames.length; ++cli) {
                            optional(p, 'FireEntityInput', [charLightNames[cli], 'Enable']);
                            optional(p, 'FireEntityInput', [charLightNames[cli], 'SetColor', '255 255 255']);
                            optional(p, 'FireEntityInput', [charLightNames[cli], 'SetLightBrightness', '7.0']);
                            optional(p, 'FireEntityInput', [charLightNames[cli], 'SetBrightness', '7.0']);
                        }
                    }
                    applyRotation();
                    applyLayout();
                    p.style.opacity = '1.0';
                    p.style.brightness = '1.20';
                    p.style.contrast = '1.08';
                    p.style.saturation = '1.14';
                    var validId = false;
                    try { validId = !!(current.id && InventoryAPI.IsValidItemID(current.id)); } catch (_) {}
                    logMsg('item id=' + current.id + ' paint=' + currentArgs.paint + ' valid=' + validId);
                    if (!current.agent) {
                        logMsg('item setter=' + itemIdMethod + ' paint setter=' +
                            (typeof p.SetItemPaintKit === 'function' ? 'SetItemPaintKit' :
                            typeof p.SetItemPaintKitIndex === 'function' ? 'SetItemPaintKitIndex' : 'faux-id'));
                    }
                    logMsg('configured pass=' + attempt + ' generation=' + current.key +
                        ' host=' + (host.id || '<root>') + ' itemValid=' + validId +
                        ' helper=' + (configuredByGameHelpers ? 'yes' : 'fallback') +
                        ' pistol=' + (current.pistolId ? 'yes' : 'no') +
                        ' camera=' + current.camera +
                        ' map=' + current.loadedMap +
                        ' model=' + (currentArgs.model || '<from-item>'));
                } catch (ce) {
                    logMsg('configure pass=' + attempt + ' error: ' + ce);
                }
            }
            state.configure = configure;
            state.generation = requestKey;
            var createdSelection = selection;
            $.Schedule(0.05, function () {
                if (!p.IsValid() || state.panel !== p || state.selection !== createdSelection) return;
                configure(1);
            });
            $.Schedule(0.2, function () {
                if (!p.IsValid() || state.panel !== p || state.selection !== createdSelection) return;
                configure(2);
            });
            // MapPlayerPreviewPanel loads its composition scene asynchronously.
            // Re-apply the player and background once the scene has settled;
            // its first frame otherwise keeps only the empty background map.
            if (selection.agent) {
                [0.75, 1.5, 3.0].forEach(function (delay, index) {
                    $.Schedule(delay, function () {
                        if (!p.IsValid() || state.panel !== p || state.selection !== createdSelection) return;
                        configure(index + 3);
                    });
                });
            }
        } catch (e) {
            destroy();
            logMsg('update exception: ' + e + (e && e.stack ? ' @ ' + e.stack : ''));
        }
    };
    function watchdog() {
        if (!root.IsValid()) {
            destroy();
            state.watching = false;
            return;
        }
        if (Date.now() - state.touched > 4000) {
            if (state.panel) logMsg('watchdog hid stale preview panel');
            destroy();
        }
        if (state.panel) $.Schedule(0.25, watchdog);
        else state.watching = false;
    }
    state.submit = function (a) {
        var receivedKey = JSON.stringify([a.kind, a.def, a.paint, a.music, a.team, a.model,
            a.wear, a.seed, a.stattrak ? 1 : 0, a.stattrak_count, a.name_tag]);
        if (state.receivedKey !== receivedKey) {
            state.receivedKey = receivedKey;
            logMsg('request received kind=' + a.kind + ' def=' + a.def + ' paint=' + a.paint + ' team=' + a.team);
        }
        try {
            state.update(a);
        } catch (submitError) {
            logMsg('request handler failed: ' + submitError);
        }
        if (state.panel && !state.watching) { state.watching = true; $.Schedule(0.5, watchdog); }
    };
})();
)MINTALY_JS";
}
