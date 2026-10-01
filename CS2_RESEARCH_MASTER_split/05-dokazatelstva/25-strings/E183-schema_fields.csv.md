<!-- split-part | CS2_RESEARCH_MASTER.md lines 101300-101368 | body-sha256 8ad16cb807ef21f4d0a3613d949a1202711e28d9683ce6140348ae2e1384ee1f -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-183"></a>

## E183. `analysis/strings/schema_fields.csv`

Bytes: 6603. SHA-256: `f859ed4d0d1cb65dfee4919f84ccccf40f99fa0e964f22fd6b921ba55564f5dc`.

```csv
proto_file,owner,field,field_number,label,protobuf_type,type_name,file_offset_hex,rva_hex,va_hex,byte_length
usercmd.proto,CInButtonStatePB,buttonstate1,1,optional,uint64,,0x00DED106,0x00DED106,0x212C40ED106,12
usercmd.proto,CInButtonStatePB,buttonstate2,2,optional,uint64,,0x00DED11C,0x00DED11C,0x212C40ED11C,12
usercmd.proto,CInButtonStatePB,buttonstate3,3,optional,uint64,,0x00DED132,0x00DED132,0x212C40ED132,12
usercmd.proto,CSubtickMoveStep,button,1,optional,uint64,,0x00DED163,0x00DED163,0x212C40ED163,6
usercmd.proto,CSubtickMoveStep,pressed,2,optional,bool,,0x00DED173,0x00DED173,0x212C40ED173,7
usercmd.proto,CSubtickMoveStep,when,3,optional,float,,0x00DED184,0x00DED184,0x212C40ED184,4
usercmd.proto,CSubtickMoveStep,analog_forward_delta,4,optional,float,,0x00DED192,0x00DED192,0x212C40ED192,20
usercmd.proto,CSubtickMoveStep,analog_left_delta,5,optional,float,,0x00DED1B0,0x00DED1B0,0x212C40ED1B0,17
usercmd.proto,CSubtickMoveStep,pitch_delta,8,optional,float,,0x00DED1CB,0x00DED1CB,0x212C40ED1CB,11
usercmd.proto,CSubtickMoveStep,yaw_delta,9,optional,float,,0x00DED1E0,0x00DED1E0,0x212C40ED1E0,9
usercmd.proto,CBaseUserCmdExecutionNotes,ignored_reason,1,optional,string,,0x00DED217,0x00DED217,0x212C40ED217,14
usercmd.proto,CBaseUserCmdPB,legacy_command_number,1,optional,int32,,0x00DED248,0x00DED248,0x212C40ED248,21
usercmd.proto,CBaseUserCmdPB,client_tick,2,optional,int32,,0x00DED267,0x00DED267,0x212C40ED267,11
usercmd.proto,CBaseUserCmdPB,prediction_offset_ticks_x256,17,optional,uint32,,0x00DED27C,0x00DED27C,0x212C40ED27C,28
usercmd.proto,CBaseUserCmdPB,buttons_pb,3,optional,message,.CInButtonStatePB,0x00DED2A2,0x00DED2A2,0x212C40ED2A2,10
usercmd.proto,CBaseUserCmdPB,viewangles,4,optional,message,.CMsgQAngle,0x00DED2C9,0x00DED2C9,0x212C40ED2C9,10
usercmd.proto,CBaseUserCmdPB,forwardmove,5,optional,float,,0x00DED2EA,0x00DED2EA,0x212C40ED2EA,11
usercmd.proto,CBaseUserCmdPB,leftmove,6,optional,float,,0x00DED2FF,0x00DED2FF,0x212C40ED2FF,8
usercmd.proto,CBaseUserCmdPB,upmove,7,optional,float,,0x00DED311,0x00DED311,0x212C40ED311,6
usercmd.proto,CBaseUserCmdPB,impulse,8,optional,int32,,0x00DED321,0x00DED321,0x212C40ED321,7
usercmd.proto,CBaseUserCmdPB,weaponselect,9,optional,int32,,0x00DED332,0x00DED332,0x212C40ED332,12
usercmd.proto,CBaseUserCmdPB,random_seed,10,optional,int32,,0x00DED348,0x00DED348,0x212C40ED348,11
usercmd.proto,CBaseUserCmdPB,mousedx,11,optional,int32,,0x00DED35D,0x00DED35D,0x212C40ED35D,7
usercmd.proto,CBaseUserCmdPB,mousedy,12,optional,int32,,0x00DED36E,0x00DED36E,0x212C40ED36E,7
usercmd.proto,CBaseUserCmdPB,pawn_entity_handle,14,optional,uint32,,0x00DED37F,0x00DED37F,0x212C40ED37F,18
usercmd.proto,CBaseUserCmdPB,subtick_moves,18,repeated,message,.CSubtickMoveStep,0x00DED3A5,0x00DED3A5,0x212C40ED3A5,13
usercmd.proto,CBaseUserCmdPB,move_crc,19,optional,bytes,,0x00DED3CF,0x00DED3CF,0x212C40ED3CF,8
usercmd.proto,CBaseUserCmdPB,consumed_server_angle_changes,20,optional,uint32,,0x00DED3E1,0x00DED3E1,0x212C40ED3E1,29
usercmd.proto,CBaseUserCmdPB,cmd_flags,21,optional,int32,,0x00DED408,0x00DED408,0x212C40ED408,9
usercmd.proto,CBaseUserCmdPB,execution_notes,22,optional,message,.CBaseUserCmdExecutionNotes,0x00DED41B,0x00DED41B,0x212C40ED41B,15
usercmd.proto,CUserCmdBasePB,base,1,optional,message,.CBaseUserCmdPB,0x00DED469,0x00DED469,0x212C40ED469,4
cs_usercmd.proto,CSGOInterpolationInfoPB,src_tick,1,optional,int32,,0x00DF13AF,0x00DF13AF,0x212C40F13AF,8
cs_usercmd.proto,CSGOInterpolationInfoPB,dst_tick,2,optional,int32,,0x00DF13C5,0x00DF13C5,0x212C40F13C5,8
cs_usercmd.proto,CSGOInterpolationInfoPB,frac,3,optional,float,,0x00DF13DB,0x00DF13DB,0x212C40F13DB,4
cs_usercmd.proto,CSGOInterpolationInfoPB_CL,frac,3,optional,float,,0x00DF1410,0x00DF1410,0x212C40F1410,4
cs_usercmd.proto,CSGOInputHistoryEntryPB,view_angles,2,optional,message,.CMsgQAngle,0x00DF1443,0x00DF1443,0x212C40F1443,11
cs_usercmd.proto,CSGOInputHistoryEntryPB,render_tick_count,4,optional,int32,,0x00DF1465,0x00DF1465,0x212C40F1465,17
cs_usercmd.proto,CSGOInputHistoryEntryPB,render_tick_fraction,5,optional,float,,0x00DF1480,0x00DF1480,0x212C40F1480,20
cs_usercmd.proto,CSGOInputHistoryEntryPB,player_tick_count,6,optional,int32,,0x00DF149E,0x00DF149E,0x212C40F149E,17
cs_usercmd.proto,CSGOInputHistoryEntryPB,player_tick_fraction,7,optional,float,,0x00DF14B9,0x00DF14B9,0x212C40F14B9,20
cs_usercmd.proto,CSGOInputHistoryEntryPB,cl_interp,12,optional,message,.CSGOInterpolationInfoPB_CL,0x00DF14D7,0x00DF14D7,0x212C40F14D7,9
cs_usercmd.proto,CSGOInputHistoryEntryPB,sv_interp0,13,optional,message,.CSGOInterpolationInfoPB,0x00DF1507,0x00DF1507,0x212C40F1507,10
cs_usercmd.proto,CSGOInputHistoryEntryPB,sv_interp1,14,optional,message,.CSGOInterpolationInfoPB,0x00DF1535,0x00DF1535,0x212C40F1535,10
cs_usercmd.proto,CSGOInputHistoryEntryPB,player_interp,15,optional,message,.CSGOInterpolationInfoPB,0x00DF1563,0x00DF1563,0x212C40F1563,13
cs_usercmd.proto,CSGOInputHistoryEntryPB,frame_number,64,optional,int32,,0x00DF1594,0x00DF1594,0x212C40F1594,12
cs_usercmd.proto,CSGOInputHistoryEntryPB,target_ent_index,65,optional,int32,,0x00DF15AA,0x00DF15AA,0x212C40F15AA,16
cs_usercmd.proto,CSGOInputHistoryEntryPB,shoot_position,66,optional,message,.CMsgVector,0x00DF15C8,0x00DF15C8,0x212C40F15C8,14
cs_usercmd.proto,CSGOInputHistoryEntryPB,target_head_pos_check,67,optional,message,.CMsgVector,0x00DF15ED,0x00DF15ED,0x212C40F15ED,21
cs_usercmd.proto,CSGOInputHistoryEntryPB,target_abs_pos_check,68,optional,message,.CMsgVector,0x00DF1619,0x00DF1619,0x212C40F1619,20
cs_usercmd.proto,CSGOInputHistoryEntryPB,target_abs_ang_check,69,optional,message,.CMsgQAngle,0x00DF1644,0x00DF1644,0x212C40F1644,20
cs_usercmd.proto,CSGOUserCmdPB,base,1,optional,message,.CBaseUserCmdPB,0x00DF1687,0x00DF1687,0x212C40F1687,4
cs_usercmd.proto,CSGOUserCmdPB,input_history,2,repeated,message,.CSGOInputHistoryEntryPB,0x00DF16A6,0x00DF16A6,0x212C40F16A6,13
cs_usercmd.proto,CSGOUserCmdPB,attack1_start_history_index,6,optional,int32,,0x00DF16D7,0x00DF16D7,0x212C40F16D7,27
cs_usercmd.proto,CSGOUserCmdPB,attack2_start_history_index,7,optional,int32,,0x00DF1700,0x00DF1700,0x212C40F1700,27
cs_usercmd.proto,CSGOUserCmdPB,left_hand_desired,9,optional,bool,,0x00DF1729,0x00DF1729,0x212C40F1729,17
cs_usercmd.proto,CSGOUserCmdPB,is_predicting_body_shot_fx,11,optional,bool,,0x00DF174B,0x00DF174B,0x212C40F174B,26
cs_usercmd.proto,CSGOUserCmdPB,is_predicting_head_shot_fx,12,optional,bool,,0x00DF1776,0x00DF1776,0x212C40F1776,26
cs_usercmd.proto,CSGOUserCmdPB,is_predicting_kill_ragdolls,13,optional,bool,,0x00DF17A1,0x00DF17A1,0x212C40F17A1,27
```
