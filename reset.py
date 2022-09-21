
Import("env")
import sys
print("-----------------------------------------------------reset.py")
print(sys.version)

# from platformio.builder.tools.pioupload import AutodetectUploadPort
# platform = env.PioPlatform()
# AutodetectUploadPort(env)
# upload_port = env.subst('$UPLOAD_PORT')
# pdir = env.subst('$PROJECT_DIR')
# print(pdir, env, upload_port)
# env.Append(
#     CFLAGS=["-Dprintf=iprintf"],
#     LDFLAGS=["-lc", "-lrdimon", "-u _printf_floatk"]
# )
Import("env")
import os
esptool_cmd = f'$PYTHONEXE  -VV' 

# Multiple actions
env.AddCustomTarget(
    name="reset",
    dependencies=None,
    actions=[
        esptool_cmd
    ],
    title="Reset ST-LINK",
    description="Resets the ESP8266 board"
)
print("OUT--------------------------------------------------reset.py")
