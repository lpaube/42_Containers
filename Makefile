containers: main.cpp vector.hpp
	gcc++ -Wall -Werror -Wextra -std=c++98 -g main.cpp -o containers

clean:
	rm -rf containers

re: clean containers
