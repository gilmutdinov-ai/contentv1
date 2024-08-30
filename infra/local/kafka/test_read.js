
const { Kafka } = require('kafkajs');
var fs = require('fs');

const topic_name = "req_urls_stream"
const client_id = "contentv1_infra_test_read"

async function testRead() {

    const kafka = new Kafka({
        clientId: client_id,
        brokers: ['kafka:9092', 'kafka:29092'],
    })

    const consumer = kafka.consumer({ groupId: 'test-group' })

    await consumer.connect()
    await consumer.subscribe({ topic: topic_name, fromBeginning: true })

    await consumer.run({
        eachMessage: async ({ topic, partition, message }) => {
            console.log({
                value: message.value.toString(),
            })
        },
    })
}

testRead()