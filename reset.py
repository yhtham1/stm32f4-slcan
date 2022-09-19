
Import("env")
import sys
print("-----------------------------------------------------reset.py")
print(sys.version)

from platformio.builder.tools.pioupload import AutodetectUploadPort
platform = env.PioPlatform()
AutodetectUploadPort(env)
upload_port = env.subst('$UPLOAD_PORT')
print(env, upload_port)

env.Append(
    CFLAGS=["-Dprintf=iprintf"],
    LDFLAGS=["-lc", "-lrdimon", "-u _printf_floatk"]
)
print("OUT--------------------------------------------------reset.py")
