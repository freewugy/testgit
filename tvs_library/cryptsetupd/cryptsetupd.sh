#!/bin/bash

#echo $0

MODEL="ST950I-LN"

cryptsetupd_pid()
{
    pid=`ps -ef | grep "cryptsetupd.py $1" | grep -v grep | awk '{print $2}'`

    echo $pid
}

stop()
{
    model=$1
    pid=`cryptsetupd_pid $model`
    if [ -z $pid ]; then
        echo "cryptsetupd : [stopped]"
    else
        kill -9 $pid
        if [ 0 = $? ]; then
            echo "cryptsetupd : [stopped]"
        fi
    fi
}

start()
{
    model=$1
    pid=`cryptsetupd_pid $model`
    if [ -z $pid ]; then
        python ./cryptsetupd.py $MODEL &
        #./cryptsetupd.pyc &
        pid=`cryptsetupd_pid`
        echo "cryptsetupd : [started] (pid : $pid)"
    else
        echo "cryptsetupd already start (pid $pid)"
    fi
}

restart()
{
    model=$1
    stop $model
    sleep 1
    if [ $model = "SH960C-LN" ]; then
        if [ -f ./system.encrypted.empty.sh960c-ln ]; then
            rm -f ./system.encrypted.empty.sh960c-ln
        fi
    fi
    sync
    start $model
}

log()
{
    model=$1
    date=`date +%Y-%m-%d`
    logfile="./log/cryptsetupd_${model}_${date}.log"
    tail -f $logfile
}

usage()
{
    echo "Usage: sudo ./cryptsetupd.sh [ST950I-LN|SH960C-LN] {start|stop|restart|status|log}"
}

if [ -z $1 ]; then
    cryptsetupd_ver=`cat cryptsetupd.py | sed -n "s/^VERSION=//p" | sed -n "s/\"//gp"`
    echo "cryptsetd.py $cryptsetupd_ver"

    usage
    exit 1
fi

if [ $1 != "ST950I-LN" -a $1 != "SH960C-LN" ]; then
    usage
    exit 1
fi


while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{$1="";print $0}'`
  case $PARAM in
      -h | --help)
          usage
          exit
          ;;
      ST950I-LN|SH960C-LN)
          MODEL=$PARAM
          ;;
      stop)
          stop $MODEL
          ;;
      status)
          pid=`cryptsetupd_pid $MODEL`
          if [ -z $pid ]; then
              echo "cryptsetupd : [stopped]"
          else
              echo "cryptsetupd : [started] (pid : $pid)"
          fi
          ;;
      start)
          start $MODEL
          ;;
      restart)
          restart $MODEL
          ;;
      log)
          log $MODEL
          ;;
      *)
          echo "ERROR: unknown parameter \"$PARAM\""
          usage
          exit 1
          ;;
  esac
  shift
done

