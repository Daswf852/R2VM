local source_path, target_cpu_id, log_path = ...

local redirect_log

local print_ = print
local function print_log(sugar, no_sugar, str)
    if redirect_log then
        redirect_log:write(no_sugar .. str .. "\n")
    else
        print_(sugar .. str)
    end
end
local function print_e(...)
    print_log("[r2asm, error]", "[r2asm] ", string.format(...))
end
local function print_w(...)
    print_log("[r2asm, warning]", "[r2asm] ", string.format(...))
end
local function print_i(...)
    print_log("[r2asm, info]", "[r2asm] ", string.format(...))
end
local print = nil

if log_path then
    local handle, err = io.open(log_path, "w")
    if handle then
        redirect_log = handle
    else
        print_w("failed redirect log: %s", err)
    end
end
local function ipairs(tbl_param)
    return function(tbl, idx)
        idx = (idx or 0) + 1
        if #tbl >= idx then
            return idx, tbl[idx]
        end
    end, tbl_param
end

local supported_models = {
    ["R216K2A"] = {
        flash_mode = "skip_one_rtl",
        ram_x = 70,
        ram_y = -99,
        ram_width = 128,
        ram_height = 16
    },
    ["R216K4A"] = {
        flash_mode = "skip_one_rtl",
        ram_x = 70,
        ram_y = -115,
        ram_width = 128,
        ram_height = 32
    },
    ["_R216VM"]={
        flash_mode = "skip_one_rtl",
        ram_x = 0,
        ram_y = 0,
        ram_width = 256, --dummy specs to allow 64KiB memory
        ramheight = 256
    },
}
local supported_flash_modes = {
    ["skip_one_rtl"] = function(model, code)
        local handle = io.open("r2.dump", "w")
        for ix = 1, #code do
            handle:write(("0x%08X\n"):format(code[ix]))
        end
        handle:close()
        return true
    end
}

local mnemonic_to_class = {
    ["adc" ] = {code = 0x25000000, class =  "2"},
    ["adcs"] = {code = 0x2D000000, class =  "2"},
    ["add" ] = {code = 0x24000000, class =  "2"},
    ["adds"] = {code = 0x2C000000, class =  "2"},
    ["and" ] = {code = 0x21000000, class =  "2"},
    ["ands"] = {code = 0x29000000, class =  "2"},
    ["bump"] = {code = 0x38000000, class =  "1"},
    ["call"] = {code = 0x3E000000, class = "1*"},
    ["cmb" ] = {code = 0x2F000000, class =  "2"},
    ["cmp" ] = {code = 0x2E000000, class =  "2"},
    ["hlt" ] = {code = 0x30000000, class =  "0"},
    ["ja"  ] = {code = 0x3100000F, class = "1*"},
    ["jae" ] = {code = 0x31000003, class = "1*"},
    ["jb"  ] = {code = 0x31000002, class = "1*"},
    ["jbe" ] = {code = 0x3100000E, class = "1*"},
    ["jc"  ] = {code = 0x31000002, class = "1*"},
    ["je"  ] = {code = 0x31000008, class = "1*"},
    ["jg"  ] = {code = 0x3100000B, class = "1*"},
    ["jge" ] = {code = 0x3100000D, class = "1*"},
    ["jl"  ] = {code = 0x3100000C, class = "1*"},
    ["jle" ] = {code = 0x3100000A, class = "1*"},
    ["jmp" ] = {code = 0x31000000, class = "1*"},
    ["jn"  ] = {code = 0x31000001, class = "1*"},
    ["jna" ] = {code = 0x3100000E, class = "1*"},
    ["jnae"] = {code = 0x31000002, class = "1*"},
    ["jnb" ] = {code = 0x31000003, class = "1*"},
    ["jnbe"] = {code = 0x3100000F, class = "1*"},
    ["jnc" ] = {code = 0x31000003, class = "1*"},
    ["jne" ] = {code = 0x31000009, class = "1*"},
    ["jng" ] = {code = 0x3100000A, class = "1*"},
    ["jnge"] = {code = 0x3100000C, class = "1*"},
    ["jnl" ] = {code = 0x3100000D, class = "1*"},
    ["jnle"] = {code = 0x3100000B, class = "1*"},
    ["jno" ] = {code = 0x31000005, class = "1*"},
    ["jns" ] = {code = 0x31000007, class = "1*"},
    ["jnz" ] = {code = 0x31000009, class = "1*"},
    ["jo"  ] = {code = 0x31000004, class = "1*"},
    ["js"  ] = {code = 0x31000006, class = "1*"},
    ["jz"  ] = {code = 0x31000008, class = "1*"},
    ["mov" ] = {code = 0x20000000, class =  "2"},
    ["nop" ] = {code = 0x31000001, class =  "0"},
    ["or"  ] = {code = 0x22000000, class =  "2"},
    ["ors" ] = {code = 0x2A000000, class =  "2"},
    ["pop" ] = {code = 0x3D000000, class =  "1"},
    ["push"] = {code = 0x3C000000, class = "1*"},
    ["recv"] = {code = 0x3B000000, class =  "2"},
    ["ret" ] = {code = 0x3F000000, class =  "0"},
    ["rol" ] = {code = 0x32000000, class =  "2"},
    ["ror" ] = {code = 0x33000000, class =  "2"},
    ["sbb" ] = {code = 0x27000000, class =  "2"},
    ["sbbs"] = {code = 0x2F000000, class =  "2"},
    ["send"] = {code = 0x3A000000, class =  "2"},
    ["shl" ] = {code = 0x34000000, class =  "2"},
    ["scl" ] = {code = 0x36000000, class =  "2"},
    ["shr" ] = {code = 0x35000000, class =  "2"},
    ["scr" ] = {code = 0x37000000, class =  "2"},
    ["sub" ] = {code = 0x26000000, class =  "2"},
    ["subs"] = {code = 0x2E000000, class =  "2"},
    ["swm" ] = {code = 0x28000000, class = "1*"},
    ["test"] = {code = 0x29000000, class =  "2"},
    ["wait"] = {code = 0x39000000, class =  "1"},
    ["xor" ] = {code = 0x23000000, class =  "2"},
    ["xors"] = {code = 0x2B000000, class =  "2"},
}
local class_to_modes = {
    ["0"] = {
        {pattern = {},                                      code = 0x00000000},
    },
    ["1"] = {
        {pattern = {"r0"},                                  code = 0x00000000},
        {pattern = {"[", "r0", "]"},                        code = 0x00400000},
        {pattern = {"[", "r16", "+", "r0", "]"},            code = 0x00C00000},
        {pattern = {"[", "r16", "-", "r0", "]"},            code = 0x00C08000},
        {pattern = {"[", "64", "]"},                        code = 0x00500000},
        {pattern = {"[", "14", "+", "r16", "]"},            code = 0x00D00000},
        {pattern = {"[", "r16", "+", "14", "]"},            code = 0x00D00000},
        {pattern = {"[", "r16", "-", "14", "]"},            code = 0x00D08000},
    },
    ["1*"] = {
        {pattern = {"r4"},                                  code = 0x00000000},
        {pattern = {"[", "r4", "]"},                        code = 0x00100000},
        {pattern = {"[", "r16", "+", "r4", "]"},            code = 0x00900000},
        {pattern = {"[", "r16", "-", "r4", "]"},            code = 0x00908000},
        {pattern = {"64"},                                  code = 0x00200000},
        {pattern = {"[", "64", "]"},                        code = 0x00300000},
        {pattern = {"[", "14", "+", "r16", "]"},            code = 0x00B00000},
        {pattern = {"[", "r16", "+", "14", "]"},            code = 0x00B00000},
        {pattern = {"[", "r16", "-", "14", "]"},            code = 0x00B08000},
    },
    ["2"] = {
        {pattern = {"r0", ",", "r4"},                       code = 0x00000000},
        {pattern = {"r0", ",", "[", "r4", "]"},             code = 0x00100000},
        {pattern = {"r0", ",", "[", "r16", "+", "r4", "]"}, code = 0x00900000},
        {pattern = {"r0", ",", "[", "r16", "-", "r4", "]"}, code = 0x00908000},
        {pattern = {"r0", ",", "64"},                       code = 0x00200000},
        {pattern = {"r0", ",", "[", "64", "]"},             code = 0x00300000},
        {pattern = {"r0", ",", "[", "14", "+", "r16", "]"}, code = 0x00B00000},
        {pattern = {"r0", ",", "[", "r16", "+", "14", "]"}, code = 0x00B00000},
        {pattern = {"r0", ",", "[", "r16", "-", "14", "]"}, code = 0x00B08000},
        {pattern = {"[", "r0", "]", ",", "r4"},             code = 0x00400000},
        {pattern = {"[", "r16", "+", "r0", "]", ",", "r4"}, code = 0x00C00000},
        {pattern = {"[", "r16", "-", "r0", "]", ",", "r4"}, code = 0x00C08000},
        {pattern = {"[", "64", "]", ",", "r0"},             code = 0x00500000},
        {pattern = {"[", "14", "+", "r16", "]", ",", "r0"}, code = 0x00D00000},
        {pattern = {"[", "r16", "+", "14", "]", ",", "r0"}, code = 0x00D00000},
        {pattern = {"[", "r16", "-", "14", "]", ",", "r0"}, code = 0x00D08000},
        {pattern = {"[", "r0", "]", ",", "64"},             code = 0x00600000},
        {pattern = {"[", "r16", "+", "r0", "]", ",", "14"}, code = 0x00E00000},
        {pattern = {"[", "r16", "-", "r0", "]", ",", "14"}, code = 0x00E08000},
        {pattern = {"[", "64", "]", ",", "40"},             code = 0x00700000},
        {pattern = {"[", "14", "+", "r16", "]", ",", "40"}, code = 0x00F00000},
        {pattern = {"[", "r16", "+", "14", "]", ",", "40"}, code = 0x00F00000},
        {pattern = {"[", "r16", "-", "14", "]", ",", "40"}, code = 0x00F08000},
    },
}
local register_to_name = {
    [ "r0"] =  0, [ "r1"] =  1, [ "r2"] =  2, [ "r3"] =  3,
    [ "r4"] =  4, [ "r5"] =  5, [ "r6"] =  6, [ "r7"] =  7,
    [ "r8"] =  8, [ "r9"] =  9, ["r10"] = 10, ["r11"] = 11,
    ["r12"] = 12, ["r13"] = 13, ["r14"] = 14, ["r15"] = 15,
                  [ "bp"] = 13, [ "sp"] = 14, [ "ip"] = 15,
}

local function assemble_source()

    local errors_encountered = false
    local function print_el(line, ...)
        print_e("line %i: %s", line, string.format(...))
        errors_encountered = true
    end
    local function print_wl(line, ...)
        print_w("line %i: %s", line, string.format(...))
    end

    local lines = {}
    local labels = {}
    local line_to_offset = {}
    local literals = {}
    local truncated = {}
    local patch_label = {}
        
    local function match_register(operands_str)
        local register, remainder = operands_str:match("^%s*(%w+)%s*(.*)%s*$")
        if not register then
            return
        end
        local register_name = register_to_name[register:lower()]
        if not register_name then
            return
        end
        return register_name, remainder
    end
    local function match_imm_impl(operands_str, line)
        local immval, remainder = operands_str:match(
            "^%s*([%-%w._']+)%s*(.*)%s*$")
        if not immval then
            return
        end
        local number = tonumber(immval)
        if number then
            return number, remainder
        end
        local label_name = immval:match("^%.?[%a_][%w_]*$")
        if not label_name then
            label_name = immval:match("^[%a_][%w_]*%.[%a_][%w_]*$")
        end
        if mnemonic_to_class[label_name] or register_to_name[label_name] then
            return
        end
        if label_name then
            return 0, remainder, label_name
        end
        local lit_idx, remainder = operands_str:match("^%s*'(%d+)'%s*(.*)%s*$")
        lit_idx = tonumber(lit_idx)
        if not lit_idx then
            return
        end
        local literal_value = 0
        local literal_data = literals[lit_idx].data
        if #literal_data == 0 then
            print_w(line, "empty literal, assuming \\0")
        end
        for ix = 1, math.min(4, #literal_data) do
            literal_value = literal_value * 0x100 + literal_data:byte(ix)
        end
        return literal_value, remainder
    end
    local function match_imm(operands_str, width, offset, line, shift)
        local number, remainder, label = match_imm_impl(operands_str, line)
        if not number then
            return
        end
        local sane_number = math.floor(number) % (2 ^ width)
        if number ~= sane_number then
            if not (width == 16
                and number >= -32768
                and number + 0x10000 == sane_number) then
                truncated[line] = width
            end
        end
        -- * Queue for later patching.
        if label then
            patch_label[offset] = {
                label = label,
                width = width,
                shift = shift,
                invoker = line
            }
        end
        return sane_number, remainder
    end
    local function match_punctuator(operands_str, punctuator)
        local nonspace, remainder = operands_str:match("^%s*(%S)%s*(.*)%s*$")
        if nonspace ~= punctuator then
            return
        end
        return remainder
    end
    local function match_dw_string(operands, line)
        local lit_idx, remainder = operands:match("^%s*\"(%d+)\"%s*(.*)%s*$")
        lit_idx = tonumber(lit_idx)
        if not lit_idx then
            return
        end
        return literals[lit_idx].data, remainder
    end

    local function match_operand_mode(operands, offset, line, mode)
        local code = mode.code
        for ix_token, token in ipairs(mode.pattern) do
            if #token == 1 then
                operands = match_punctuator(operands, token)
            else
                local bits
                local pos = tonumber(token:sub(2))
                if token:find("^r") then
                    bits, operands = match_register(operands, token)
                elseif token:find("^6") then
                    bits, operands = match_imm(operands, 16, offset, line, pos)
                elseif token:find("^1") then
                    bits, operands = match_imm(operands, 11, offset, line, pos)
                elseif token:find("^4") then
                    bits, operands = match_imm(operands, 4, offset, line, pos)
                end
                if bits then
                    code = code + bits * 2 ^ pos
                end
            end
            if not operands then
                return
            end
        end
        if operands:match("^%s*$") then
            return code
        end
    end

    -- * Try to load the source first, bail out if we can't.
    local source
    do
        local handle, err = io.open(source_path, "r")
        if not handle then
            print_e("failed to open source: %s", err)
            return
        end
        source = handle:read("*a"):gsub("\r\n?", "\n")
        handle:close()
    end

    do
        local last_global_label = false
        local ix_line = 0
        for line in (source .. "\n"):gmatch("([^\n]*)\n") do
            ix_line = ix_line + 1

            line = line:gsub("\"([^\"]*)\"", function(cap)
                table.insert(literals, {
                    data = cap,
                    line = ix_line
                })
                return "\"" .. #literals .. "\""
            end):gsub("'([^']*)'", function(cap)
                table.insert(literals, {
                    data = cap,
                    line = ix_line
                })
                return "'" .. #literals .. "'"
            end):gsub(";.*$", "")

            line = line:gsub("%s*(%.?[%a_][%w_]*)%s*:", function(cap)
                local label_name
                if cap:find("^%.") then
                    -- * Handle local labels.
                    if last_global_label then
                        label_name = last_global_label .. cap
                    else
                        print_el(line, "local label without global label")
                    end
                else
                    label_name = cap
                    last_global_label = label_name
                end
                if label_name then
                    labels[label_name] = ix_line
                end
                return ""
            end)

            table.insert(lines, {
                str = line,
                global_label = last_global_label
            })
        end
    end

    for ix_literal, literal in ipairs(literals) do
        literals[ix_literal].data = literal.data
            :gsub("\\a", "\a")
            :gsub("\\b", "\b")
            :gsub("\\f", "\f")
            :gsub("\\n", "\n")
            :gsub("\\r", "\r")
            :gsub("\\t", "\t")
            :gsub("\\v", "\v")
            :gsub("\\x(.?.?)", function(cap)
                local code = #cap == 2 and tonumber(cap, 16)
                if not code then
                    print_el(literal.line, "invalid escape sequence \\x%s", cap)
                    return "\\x" .. cap
                end
                return string.char(code)
            end)
            :gsub("\\(%d%d?%d?)", function(cap)
                local code = tonumber(cap)
                if code > 255 then
                    print_el(literal.line, "invalid escape sequence \\%s", cap)
                    return "\\" .. cap
                end
                return string.char(code)
            end)
            :gsub("\\.", function(cap)
                print_el(literal.line, "invalid escape sequence %s", cap)
                return cap
            end)
            :gsub("\\\\", "\\")
    end

    local machine_code = {}
    local commit_value, pad_holes
    do
        local max_pos = 0
        function commit_value(pos, value)
            pos = pos + 1
            machine_code[pos] = value
            if max_pos < pos then
                max_pos = pos
            end
        end
        function pad_holes()
            for ix = 1, max_pos do
                machine_code[ix] = machine_code[ix] or 0x20000000
            end
        end
    end

    -- * Translate lines.
    do
        local ram_offset = 0
        for ix_line, line in ipairs(lines) do
            line_to_offset[ix_line] = ram_offset
            if line.str:find("%S") then
                local mnemonic, operands = line.str:match(
                    "^%s*(%w+)%s*(.*)%s*$")
                if mnemonic then
                    mnemonic = mnemonic:lower()
                    local op_code_class = mnemonic_to_class[mnemonic]
                    if op_code_class then
                        local op_code = op_code_class.code
                        local op_class = op_code_class.class
                        local operand_modes = class_to_modes[op_class]
                        local mode_code

                        local matching_mode, operand_matches
                        for ix_mode, mode in ipairs(operand_modes) do
                            mode_code = match_operand_mode(
                                operands,
                                ram_offset,
                                ix_line,
                                mode
                            )
                            if mode_code then
                                break
                            end
                        end
                        if mode_code then
                            commit_value(ram_offset, mode_code + op_code)
                            ram_offset = ram_offset + 1
                        else
                            print_el(
                                ix_line,
                                "invalid operand list (class %s)",
                                op_class
                            )
                        end

                    elseif mnemonic == "dw" then
                        -- * Handle dw directive.
                        while true do
                            local number, remainder = match_imm(operands, 16,
                                ram_offset, ix_line, 0)
                            if number then
                                commit_value(ram_offset, 0x20000000 + number)
                                ram_offset = ram_offset + 1
                                operands = remainder
                            else
                                local literal_data, remainder = match_dw_string(
                                    operands, ix_line)
                                if literal_data then
                                    for ix = 1, #literal_data do
                                        commit_value(ram_offset, 0x20000000 +
                                            literal_data:byte(ix))
                                        ram_offset = ram_offset + 1
                                    end
                                    operands = remainder
                                else
                                    print_el(ix_line,
                                        "expected immediate or string literal")
                                    break
                                end
                            end

                            if operands:match("^%s*$") then
                                break
                            end
                            operands = match_punctuator(operands, ",")
                            if not operands then
                                print_el(ix_line, "expected ','")
                            end
                        end

                    elseif mnemonic == "org" then
                        local new_origin = tonumber(operands)
                        if new_origin then
                            new_origin = math.floor(new_origin)
                            if new_origin < 0 then
                                print_el(ix_line, "origin below 0")
                            else
                                ram_offset = new_origin
                            end
                        else
                            print_el(ix_line, "malformed number")
                        end

                    else
                        print_el(ix_line, "unknown mnemonic")
                    end
                else
                    print_el(ix_line, "expected mnemonic, 'dw' or 'org'")
                end
            end
        end
    end

    for offset, label_data in pairs(patch_label) do
        local label_name = label_data.label
        if label_name:find("^%.") then
            local global_label = lines[label_data.invoker].global_label
            if global_label then
                label_name = global_label .. label_name
            else
                print_el(label_data.invoker, "local label without global label")
                label_name = false
            end
        end
        if label_name then
            local label_line = labels[label_name]
            if label_line then
                local address = line_to_offset[label_line]
                local shift = label_data.shift
                if address >= 2 ^ label_data.width then
                    truncated[label_data.invoker] = width
                end
                address = (address % 2 ^ label_data.width) * 2 ^ shift
                machine_code[offset + 1] = machine_code[offset + 1] + address
            else
                print_el(label_data.invoker, "unknown label %s", label_name)
            end
        end
    end

    for line, width in pairs(truncated) do
        print_wl(line, "immediate value truncated to %i bits", width)
    end

    if errors_encountered then
        return
    end

    pad_holes()
    return machine_code
end

xpcall(function()

    local target_model_number = "R216K4A"

    local machine_code = assemble_source()
    if not machine_code then
        return
    end

    local flash_mode = supported_models[target_model_number].flash_mode
    local flasher = supported_flash_modes[flash_mode]
    if not flasher then
        print_e("flash mode not supported")
        print_i("this is a serious problem in the assembler")
        return
    end
    if not flasher(target_model_number, machine_code) then
        return
    end

    print_i("source assembled and flashed without errors")

end, function(err)
    print_e(err)
    print_e(debug.traceback())
    print_i("this is a serious problem in the assembler")
end)

if redirect_log then
    print_("[r2asm, info] log written to " .. log_path)
    redirect_log:close()
end
print_i("finished assembling")