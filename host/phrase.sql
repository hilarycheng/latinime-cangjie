create table phrase (key integer, phrase text, frequency integer, length int);
.separator ","
.import phrase.csv phrase
create index phrase_index on phrase(key);
create index phrase_len_index on phrase(key, length);

