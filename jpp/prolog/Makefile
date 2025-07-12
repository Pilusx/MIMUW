run:
	python3 sokoban.py
	python3 sokoban_verifier.py

env_get:
	virtualenv -p python3 sokoban_venv

env_source:
	#Run manually
	#source sokoban_venv/bin/activate

get_gym: env_source
	pip3 install pyswip pandas scipy==1.1.0
	git submodule add -f https://gitlab.com/awarelab/gym-sokoban.git gym
	cd gym && pip install -e .
