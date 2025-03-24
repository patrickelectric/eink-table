import subprocess
print("Generating sprites...")
subprocess.run(["/usr/bin/python3", "src/sprites/generate-sprites.py", "src/sprites"], check=True)
print("Sprites generated!")