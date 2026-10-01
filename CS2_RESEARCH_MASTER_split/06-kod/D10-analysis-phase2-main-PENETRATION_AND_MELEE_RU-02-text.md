<!-- split-kod | 04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md` (язык `text`)

[← исходная часть](../04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md`; в самой части блок остаётся на месте. Символов: 1032.

````text
state.damage = request.float_at_0x34
state.parameters = request.fields_0x38_0x3C, caller_parameters
request.float_at_0x48 = 0

for index in [0, segment_count):
    segment = begin + index * 0x18
    before = state.damage
    stop = EXTERNAL_0x7FFCEF4A8DB0(workspace, state, segment, request.field_0x40)

    if segment.flags_at_0x14 & 1:
        request.float_at_0x48 += before - state.damage

    if stop or state.damage < request.float_at_0x44:
        truncate_segment_count_to(index + 1)
        return null_pair

    if not (segment.flags_at_0x14 & 1):
        object = workspace.object_array + segment.u16_at_0x12 * 0x38
        if object.id_at_0x2C == request.id_at_0x20
           and object.byte_at_0x33 == 1
           and object.pointer_at_0x10 != null
           and object.pointer_at_0x10.u32_at_0x38 < 12:
            segment.float_at_0x08 = state.damage
            segment.field_at_0x0C = state.caller_parameter
            truncate_segment_count_to(index + 1)
            return (object, segment)

return null_pair
````
