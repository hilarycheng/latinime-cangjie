create table phrase (key integer, phrase text, frequency integer, length int);
.separator ","
.import phrase.csv phrase
create index phrase_index on phrase(key);
create index phrase_len_index on phrase(key, length);

create table phrase_single (key integer, phrase integer, frequency integer, length int);
.separator ","
.import phrase_single.csv phrase
create index phrase_single_index on phrase_single(key);
create index phrase_single_len_index on phrase_single(key, length);

