--[[ PCAN CAN TRC file recorder

Author:  Michael Fitzmayer
License: Public domain

--]]

local initial_timestamp_us
local trace_filename
local message_number = 1

local function generate_trace_filename()
    local timestamp = os.date("%Y%m%d_%H%M%S")
    return string.format("trace_%s.trc", timestamp)
end

local function write_trc_header(start_time)
    local header = string.format(";$FILEVERSION=1.1\n;$STARTTIME=%.6f\n;\n", start_time)
    header = header .. string.format(";   Start time: %s\n", os.date("%d/%m/%Y %H:%M:%S.0"))
    header = header .. ";   Generated by CANopenTerm\n;\n"
    header = header .. ";   Message Number\n"
    header = header .. ";   |         Time Offset (ms)\n"
    header = header .. ";   |         |        Type\n"
    header = header .. ";   |         |        |        ID (hex)\n"
    header = header .. ";   |         |        |        |     Data Length\n"
    header = header .. ";   |         |        |        |     |   Data Bytes (hex) ...\n"
    header = header .. ";   |         |        |        |     |   |\n"
    header = header .. ";---+--   ----+----  --+--  ----+---  +  -+ -- -- -- -- -- -- --\n"

    local file = assert(io.open(trace_filename, "w"))
    file:write(header)
    file:close()
end

local function write_to_trc(timestamp_ms, timestamp_fraction, id, length, data, message_number)
    local file = assert(io.open(trace_filename, "a"))

    local line = string.format("%5d) %10.1f  Rx     %08X  %d  ", message_number, timestamp_ms + (timestamp_fraction or 0) / 1000, id, length)

    if data then
        for i = 1, length do
            line = line .. string.format("%02X ", data:byte(i))
        end
    end

    line = line .. "\n"
    file:write(line)
    file:close()
end

print("\nTime         CAN-ID  Length  Data")

trace_filename = generate_trace_filename()

while not key_is_hit() do
    local id, length, data, timestamp_us = can_read()

    if data then
        if not initial_timestamp_us then
            initial_timestamp_us = timestamp_us
            write_trc_header(initial_timestamp_us / 1000000)
        end

        local elapsed_us = timestamp_us - initial_timestamp_us

        local timestamp_ms = math.floor(elapsed_us / 1000)
        local timestamp_fraction = math.floor(((elapsed_us / 1000) % 1) * 1000)

        io.write(string.format("%6d.%03d   %03X     %1d       ", timestamp_ms, timestamp_fraction, id, length))

        for i = 1, length do
            io.write(string.format("%02X ", data:byte(i)))
        end

        io.write("\n")

        -- Write to .trc file
        write_to_trc(timestamp_ms, timestamp_fraction, id, length, data, message_number)
        message_number = message_number + 1
    end
end

print(string.format("\nSaved as %s", trace_filename))
