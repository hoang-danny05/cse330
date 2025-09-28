savedcmd_producer_consumer.mod := printf '%s\n'   producer_consumer.o | awk '!x[$$0]++ { print("./"$$0) }' > producer_consumer.mod
