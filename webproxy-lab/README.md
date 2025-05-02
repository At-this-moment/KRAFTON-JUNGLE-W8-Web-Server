####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text

####################################################################
# CS:APP 프록시 랩
#
# 학생 소스 파일
####################################################################

이 디렉토리에는 CS:APP 프록시 실습에 필요한 파일이 포함되어 있습니다.
Lab.

proxy.c
csapp.h
csapp.c
    이들은 시작 파일입니다. csapp.c와 csapp.h는
    에 설명되어 있습니다. 

    이 파일은 원하는 대로 변경할 수 있습니다.  그리고 다음과 같이 할 수 있습니다.
    원하는 파일을 추가로 생성하여 전달할 수 있습니다.

    'port-for-user.pl' 또는 'free-port.sh'를 사용하여 프록시 또는 프록시를 위한
    프록시 또는 작은 서버를 위한 고유 포트를 생성하세요. 

메이크파일
    프록시 프로그램을 빌드하는 메이크파일입니다.  “make”
    를 입력하여 솔루션을 빌드하거나, “make clean”을 입력한 후 “make”를 입력하여
    새로 빌드합니다. 

    전달할 타르파일을 만들려면 “make handin”을 입력합니다.
    을 입력합니다. 원하는 방식으로 수정할 수 있습니다. 강사는 여러분의
    메이크파일을 사용하여 소스에서 프록시를 빌드합니다.

port-for-user.pl
    특정 사용자를 위한 임의의 포트를 생성합니다.
    용도로 임의의 포트를 생성합니다: ./port-for-user.pl <userID>

free-port.sh
    프록시 또는 작은 포트를 위해 사용할 수 있는 사용하지 않는 TCP 포트를
    프록시 또는 작은 포트에 사용할 수 있습니다. 
    사용법: ./free-port.sh

driver.sh
    기본, 동시성 및 캐시를 위한 자동 그레이더.        
    사용법 ./driver.sh

nop-server.py
     자동 그레이더용 헬퍼.         

tiny
    CS:APP 텍스트의 작은 웹 서버



Translated with DeepL.com (free version)