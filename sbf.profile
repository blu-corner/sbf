export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib

case "$(arch)" in
    x86_64)
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/thirdparty/libevent/linux64/lib/ 
    ;;
esac

export PATH=$PATH:$PWD/install/bin
