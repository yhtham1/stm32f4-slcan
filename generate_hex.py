
Import("env")
import sys
print("-----------------------------------------------------generate_hex.py")
# Custom HEX from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", 
        "$BUILD_DIR/${PROGNAME}.elf", "${PROGNAME}.hex"
    ]), "Building ${PROGNAME}.hex")
)

print("OUT--------------------------------------------------generate_hex.py")
