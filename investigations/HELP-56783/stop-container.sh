docker stop $(docker container ls | grep help-56783 | awk '{print $1}' -)
