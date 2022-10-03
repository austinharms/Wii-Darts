import sys

for path in sys.argv[2:-1]:
	base_path = sys.argv[1]
	new_path = base_path.replace(path, sys.argv[-1])
	if new_path != base_path:
		print(new_path)
		break