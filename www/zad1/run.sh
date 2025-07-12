source py/bin/activate
python3 download.py data.json
python3 generator.py data.json
pylint *.py --disable=locally-disabled
deactivate
