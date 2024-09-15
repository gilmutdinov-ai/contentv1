
Читать топик руками
        /usr/bin/kafka-console-consumer --bootstrap-server localhost:9092 --topic global_fetch_queue --from-beginning

Как создавать топики       
       kafka-topics --bootstrap-server kafka:29092 --create --if-not-exists --topic global_fetch_queue --replication-factor 1 --partitions 1


        https://stackoverflow.com/questions/64865361/docker-compose-create-kafka-topics

       