# Zenit

Zenit is an experimental project to build a high-level programming language with the following characteristics:

- Strongly typed
- Immutable by default
- Multi-paradigm
- Lexical scoped
- Structural and nominal typing mixed through interfaces

## Development

The current focus comprehends adding basic constructions for control flow and work on the type system.

#### Target platforms

Currently the compiler targets one CPU: the MOS 6502 processor (particularly the version present in the Nintendo Entertainment System).

One possible future target could be WebAssembly.

#### Development process

Every time a new feature is added to the language, the development process goes from the front-end to the back-end, so each feature produce changes throughout the pipeline. Being an experimental project, working this way allows an organic growth and the project remains managable.

The [tests folder](https://github.com/lbrugnara/zenit/blob/master/tests) contains unit tests for the different parts of the compiler, including tests for the intermediate representation and the machine code generated for the MOS 6502.

#### Syntactic Analysis

- *Lexer*: Handcrafted lexer, nothing special
- *Parser*: A recursive descendant parser implementation for the [Zenit grammar](https://github.com/lbrugnara/zenit/blob/master/docs/grammar.txt).

#### Semantic Analysis

The semantic analysis so far comprehends 4 passes:

- *Symbol resolution*: This pass register all the user defined symbols and ensures all the symbols referenced in the sources are valid.
- *Type inference*: This pass infer the type of the different expressions that do not have type information or are partially resolved by the symbol resolution pass.
- *Type check*: This pass ensures all the operations between types are correct.

#### Intermediate representation

The Zenit Intermediate Representation (ZIR) aims to be a SSA-based intermediate representation, but the work done at the moment is far away from that goal.

#### Backend

The MOS 6502 target currently supports code generation for the DATA segment and a basic startup routine that is placed in the CODE segment to initialize global variables. The first address of the startup routine is transparently used as the reset vector, and the final instruction of the startup routine is a jump to the first address of the DATA segment (user's reset vector jump address).

### Next steps

- Keep adding language constructs
- Improve ZIR structure and organization to apply optimizations, but also for the code generation process.

## How does it look?

The following snippet contains the currently supported features of the language:

```zenit

// Bool literals
var b1 = true;
var b2 = false;

// Integer literals: decimals
var n1 = 1;

// Integer literals: hexadecimals
var n2 = 0xFF;

// Array literals
var a = [ 1, 2, 3 ];

// Unnamed struct literals
var s = { x: 1, y: 2, z: 3 };

// Implicit cast: x1 equals to 0xFF
var x1 : uint8 = cast(0x1FF);
// Explicit cast: x2 equals to 0xFF
var x2 = cast(0x1FF : uint8);

// References
var n1ref = &n1;
var n2ref : &uint8 = &n2;

// Attributes
#[NES(address: 0x2000)]
var PPUCTRL : uint8 = 1;

#[NES(segment: zp, address: 0)]
var spaceship : uint8 = 1;

// Multiple attributes
#[a1]
#[a2()]
#[a3(k: v)]
#[a4(k: v, k2: v2)]
var variable = 0;

// Blocks
{
    var a = { a: 1 };
    var b = { b: 2 };
}

// Conditionals
if (b) { 
    var x = 1; 
} else { 
    var x = 2; 
}

// Definition of a user type. There is no need to forward delcare it before
// the usage
struct A { 
    a: uint8; 
}

// Variable definition. The rvalue is a named struct initializer
var a1 = A { a: 2 };

// Variable definition. The rvalue is an unnamed struct initializer. Variable a2 is of type struct A. An unnamed struct can be casted to a named struct if its members are compatible.
var a2 : A = { a: 2 };

// Assignments: Both a3 and a4 are of type 'A'
var a3 = a1;
var a4 = a2;

// Definition of a user type.
struct B { 
    a: A;
}

// Variable definition: All the following variants of initializing struct B are the same and produce the same objects:
// Named struct literal (B) initializing its 'a' member from an unnamed struct literal (A)
var b1 = B { 
    a: { a: 2 } 
};

// Unnamed struct literal (B) initializing its 'a' member from an unnamed struct literal (A)
var b2 : B = { 
    a: { a: 2 } 
};

// Named struct literal (B) initializing its 'a' member from a named struct literal (A)
var b3 = B { 
    a: A { a: 2 } 
};

// Unnamed struct literal (B) initializing its 'a' member from a named struct literal (A)
var b4 : B = { 
    a: A { a: 2 } 
};

// Definition of a user type.
struct C { 
    a: A; 
    b: B; 
    c: uint8; 
}

// Variable definition: all the struct literals are unnamed structs casted to the particular type of the C member
var c1 = C { 
    a: { a: 2 }, 
    b: { a: { a: 3} }, 
    c: 1 
};

```



The following is an example that (ab)uses machine code instructions and benefits of the MOS 6502 interrupt vector to create a "program" for the NES console:

```zenit
/**
 *  Each array variable contains assembled instructions for the NES CPU. By means of the special 'NES' attribute
 *  and particularly with the 'address' property we ask Zenit's NES backend to store each array in certain addresses:
 *
 *  - The 'bg_color' initialized to 0x20 is a red background, changing it to 0x40 should make it green.
 *  - The 'reset' array is the routine called when the reset interrupt occurs. It prepares the PPU and then loops forever.
 *  - The 'nmi' array is the routine called when a non-maskable interrupt occurs. This routine changes the background color on each call.
 *  - The 'vectors' array tells the CPU to use addresses 0x8000 and 0x8017 for the reset and NMI interrupts handler, respectively.
 */

#[NES(address: 0x00)]           // bg_color is stored in the zero page
var bg_color = 0x20;

#[NES(address: 0x8000)]         // Reset handler
var reset = [
    0x78,                       // SEI           Disable IRQs
    0xD8,                       // CLD           Disable decimal mode
    0xA2, 0x00,                 // LDX #$00
    0x2C, 0x02, 0x20,           // BIT $2002     vwait1
    0x10, 0xFB,                 // BPL $0014
    0x2C, 0x02, 0x20,           // BIT $2002     vwait2
    0x10, 0xFB,                 // BPL $0014
    0xA9, 0x88,                 // LDA #$88      Enable NMI
    0x8D, 0x00, 0x20,           // STA $2000
    0x4C, 0x13, 0x80,           // JMP $8013     Jump forever
];

#[NES(address: 0x8016)]         // NMI handler
var nmi : []uint8 = [
    0xA6, cast(&bg_color),      // LDX $00       Load bg_color to X
    0xE8,                       // INX           Increment X
    0x86, 0x00,                 // STX $00       Store X to bg_color
    0x8E, 0x01, 0x20,           // STX $2001     Write PPUMASK
    0x40,                       // RTI
];

#[NES(address: 0xFFFA)]
var vectors : []uint16 = [
    cast(&nmi),                 // NMI
    cast(&reset),               // Reset
];
```

## License

MIT