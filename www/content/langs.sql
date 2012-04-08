DROP TABLE IF EXISTS langs;


CREATE TABLE langs (code VARCHAR PRIMARY KEY, engname VARCHAR, selfname VARCHAR);
INSERT INTO langs VALUES ('a',   'Arabic',      'العربية');
INSERT INTO langs VALUES ('af',  'Afrikaans',   'Afrikaans');
INSERT INTO langs VALUES ('al',  'Albanian',    'Shqip');
INSERT INTO langs VALUES ('bl',  'Bulgarian',   'Български');
INSERT INTO langs VALUES ('cn',  'Chichewa',    'Chichewa');
INSERT INTO langs VALUES ('chs', 'Chinese Simplified', '汉语（简化字）');
INSERT INTO langs VALUES ('ch',  'Chinese Traditional', '漢語（繁體字）');
INSERT INTO langs VALUES ('cw',  'Cibemba',     'Cibemba');
INSERT INTO langs VALUES ('c',   'Croatian',    'Hrvatski');
INSERT INTO langs VALUES ('b',   'Czech',       'Česky');
INSERT INTO langs VALUES ('d',   'Danish',      'Dansk');
INSERT INTO langs VALUES ('o',   'Dutch',       'Nederlands');
INSERT INTO langs VALUES ('e',   'English',     'English');
INSERT INTO langs VALUES ('fi',  'Finnish',     'Suomi');
INSERT INTO langs VALUES ('f',   'French',      'Français');
INSERT INTO langs VALUES ('ge',  'Georgian',    'ქართული');
INSERT INTO langs VALUES ('x',   'German',      'Deutsch');
INSERT INTO langs VALUES ('g',   'Greek',       'Ελληνική');
INSERT INTO langs VALUES ('h',   'Hungarian',   'Magyar');
INSERT INTO langs VALUES ('in',  'Indonesian',  'Bahasa Indonesia');
INSERT INTO langs VALUES ('i',   'Italian',     'Italiano');
INSERT INTO langs VALUES ('j',   'Japanese',    '日本語');
INSERT INTO langs VALUES ('ko',  'Korean',      '한국어');
INSERT INTO langs VALUES ('li',  'Lingala',     'Lingala');
INSERT INTO langs VALUES ('mg',  'Malagasy',    'Malagasy');
INSERT INTO langs VALUES ('mt',  'Maltese',     'Malti');
INSERT INTO langs VALUES ('n',   'Norwegian',   'Norsk');
INSERT INTO langs VALUES ('oss', 'Ossetian',    'Иронау');
INSERT INTO langs VALUES ('p',   'Polish',      'Polski');
INSERT INTO langs VALUES ('t',   'Portuguese',  'Português');
INSERT INTO langs VALUES ('m',   'Romanian',    'Română');
INSERT INTO langs VALUES ('u',   'Russian',     'Русский');
INSERT INTO langs VALUES ('sb',  'Serbian',     'Српски');
INSERT INTO langs VALUES ('su',  'Sesotho',     'Sesotho');
INSERT INTO langs VALUES ('sv',  'Slovenian',   'V Slovenščini');
INSERT INTO langs VALUES ('s',   'Spanish',     'Español');
INSERT INTO langs VALUES ('sr',  'Sranantongo', 'Sranantongo');
INSERT INTO langs VALUES ('sw',  'Swahili',     'Kiswahili');
INSERT INTO langs VALUES ('z',   'Swedish',     'Svenska');
INSERT INTO langs VALUES ('tg',  'Tagalog',     'Tagalog');
INSERT INTO langs VALUES ('tn',  'Tswana',      'Setswana');
INSERT INTO langs VALUES ('tk',  'Turkish',     'Türkçe');
INSERT INTO langs VALUES ('xo',  'Xhosa',       'IsiXhosa');
INSERT INTO langs VALUES ('yr',  'Yoruba',      'Yorùbá');
INSERT INTO langs VALUES ('zu',  'Zulu',        'IsiZulu');
INSERT INTO langs VALUES ('lv',  'Latvian',     'Latviešu valoda');

-- INSERT INTO langs VALUES ('bel', 'Belarusian',  'Беларуская мова');


VACUUM;
