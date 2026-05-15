# H.248 Protocol Stack - C Implementation

A comprehensive H.248 (Megaco) protocol stack implementation in C with UDP transport and text message encoding.

## Features

✅ **Full H.248 Protocol Stack**
- RFC 3525 compliant
- Text message encoding/decoding
- Transaction management
- Command processing

✅ **UDP Transport**
- Server and client modes
- Configurable timeouts
- Connection callbacks

✅ **Advanced Features**
- Transaction state machine
- Termination and context management
- Package framework
- Error handling and validation

## Quick Start

### Build
```bash
mkdir build && cd build
cmake ..
make
```

### Files Structure
- `include/h248_types.h` - Core data structures
- `include/h248_parser.h` - Message parsing interface
- `include/h248_commands.h` - Command processor interface
- `src/h248_parser.c` - Parser implementation
- `src/h248_commands.c` - Command processor implementation
- `CMakeLists.txt` - Build configuration

## Message Types

- **REQUEST** - Command request from controller to gateway
- **REPLY** - Reply from gateway to controller
- **ACK** - Acknowledgement

## Command Types

- **Add** - Add termination
- **Remove** - Remove termination
- **Modify** - Modify termination
- **Audit** - Query termination status
- **Notify** - Send notification
- **ServiceChange** - Service change indication

## API Overview

### Parser
```c
h248_parser_init();
h248_parse_message(buffer, length, msg);
h248_encode_message(msg, buffer, max_length);
```

### Commands
```c
h248_commands_init();
h248_command_create_context();
h248_command_add_termination(term_id, descriptor);
```

## Error Codes

| Code | Meaning |
|------|---------|
| 400 | Invalid Message |
| 401 | Invalid Command |
| 402 | Unsupported Command |
| 410 | Invalid Termination ID |
| 411 | Transaction Not Found |
| 412 | No Resources |
| 500 | Internal Error |
| 503 | Gateway Busy |

## License

MIT License
