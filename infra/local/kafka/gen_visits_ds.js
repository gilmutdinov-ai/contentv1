
const { Kafka } = require('kafkajs');
var fs = require('fs');
const { parse } = require('csv-parse');
var process = require('process');

/*

from url list (newsletters) generate dataset
[ {url, ts} ]
    url - normal distribution (generate duplicates)
    ts - (now-ts_interval_secs, now] - uniform distribution

*/

const workspace_dir = "/sources/contentv1";
const infn = workspace_dir + "/data/ds/newsletters/dump-2020-12-15.csv";
const outfn = workspace_dir + "/data/ds/gened_visits_dataset.json";

const ts_interval_secs = 3600 * 24 * 14; // 2 weeks
const topic_name = "req_urls_stream"
const client_id = "contentv1_infra_load_ns_ds"
const limit = 10000; // may -1

async function testWrite() {

    const kafka = new Kafka({
        clientId: client_id,
        brokers: ['kafka:9092', 'kafka:29092'],
    })

    const producer = kafka.producer()

    await producer.connect()
    await producer.send({
        topic: 'test-topic',
        messages: [
            { value: 'Hello KafkaJS user!' },
        ],
    })

    await producer.disconnect()
}

function readNewsletters(infn, onFin) {

    var urls = [];
    fs.createReadStream(infn)
        .pipe(parse({ delimiter: ',' }))
        .on('data', function (csvrow) {
            let url = csvrow[0];
            if (url.length > 4) {
                urls.push(url);
            }
        })
        .on('end', function () {
            onFin(urls);
        });
}

function randn_bm(min, max, skew) {
    let u = 0, v = 0;
    while (u === 0) u = Math.random() //Converting [0,1) to (0,1)
    while (v === 0) v = Math.random()
    let num = Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v)

    num = num / 10.0 + 0.5 // Translate to 0 -> 1
    if (num > 1 || num < 0)
        num = randn_bm(min, max, skew) // resample between 0 and 1 if out of range

    else {
        num = Math.pow(num, skew) // Skew
        num *= max - min // Stretch to fill range
        num += min // offset to min
    }
    return num
}

function unixTimestamp() {
    return Math.floor(Date.now() / 1000)
}

function getRandomInt(max) {
    return Math.floor(Math.random() * max);
}

function randomTsLastPeriod(secs) {
    return unixTimestamp() - getRandomInt(secs);
}

function buildDsDistribution(urls, ts_interval_secs, limit) {
    var ds = [];
    // Normal distribution
    for (let i = 0; i < urls.length; i++) {
        let nsamples = randn_bm(0, 10, 3) * 5;
        for (let j = 0; j < nsamples; j++) {
            ds.push({ url: urls[i], ts: randomTsLastPeriod(ts_interval_secs) });
            if (limit != -1 && ds.length >= limit)
                return ds;
        }
    }
    ds.sort(function (a, b) {
        return a.ts < b.ts
    });
    return ds;
}

function convertDsToKafkaMessages(ds) {
    var m = [];
    for (let i = 0; i < ds.length; i++) {
        m.push({ value: JSON.stringify(ds[i]) });
    }
    return m;
}

async function writeDatasetKafka(topic_name, dataset) {
    const kafka = new Kafka({
        clientId: client_id,
        brokers: ['kafka:9092', 'kafka:29092'],
    })

    const producer = kafka.producer()

    await producer.connect()
    console.log('start writing to kafka')

    let mess = convertDsToKafkaMessages(dataset);
    var batch = [];
    for (var i = 0; i < mess.length; i++) {
        batch.push(mess[i]);
        if (i % 100 == 0) {

            await producer.send({
                topic: topic_name,
                messages: batch,
            })
            batch = []
            console.log('write ', i, '/', mess.length)
        }
    }
    console.log('write finished')

    await producer.disconnect()
}

async function writeDatasetFile(outfn, dataset) {

    var f = fs.createWriteStream(outfn);
    for (var i = 0; i < dataset.length; ++i) {
        f.write(JSON.stringify(dataset[i]));
        f.write("\n");
    }
}

async function genAndWriteDatasetKafka(infn, topic_name, limit) {

    readNewsletters(infn, function (urls) {
        console.log('original dataset size: ', urls.length);
        dataset = buildDsDistribution(urls, ts_interval_secs, limit);
        console.log('dataset resampled with normal dist size: ', dataset.length)
        writeDatasetKafka(topic_name, dataset);
    });
}


async function genAndWriteDatasetFile(infn, outfn, limit) {

    readNewsletters(infn, function (urls) {
        console.log('original dataset size: ', urls.length);
        dataset = buildDsDistribution(urls, ts_interval_secs, limit);
        console.log('dataset resampled with normal dist size: ', dataset.length)
        writeDatasetFile(outfn, dataset);
    });
}

if (process.argv.length > 2) {
    if (process.argv[2] == "kafka") {
        console.log('will write to kafka..');
        genAndWriteDatasetKafka(infn, topic_name, limit);
    } else if (process.argv[2] == "file") {
        console.log('will write to file..');
        genAndWriteDatasetFile(infn, outfn, limit);
    } else {
        console.log('could not parse cli args, usage: node infra/local/kafka/gen_visits_ds.js kafka|file');
        process.exit(1);
    }
}
else {
    console.log('will write to kafka..');
    genAndWriteDatasetKafka(infn, topic_name, limit);
}
//testWrite()
