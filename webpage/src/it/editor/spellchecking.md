# Controllo ortografico

Fai correggere i tuoi errori di ortografia!

::: tip Info
QOwnNotes ha un Dictionary Manager integrato (nel menu Modifica) che puoi usare per scaricare dizionari o cancellarli (se ti manca la tua lingua vedi sotto su come richiedere l'aggiunta di un dizionario). Se desideri scaricare i dizionari manualmente, puoi seguire le istruzioni seguenti.
:::

- Sotto **Linux** puoi installare i dizionari Hunspell per la tua lingua usando il gestore pkg della tua distribuzione
    - Se è necessario aggiungere altri file di lingua, è possibile farlo in `~/.local/share/hunspell` or `/usr/share/hunspell`.
    - Se stai usando il pacchetto Snap di QOwnNotes puoi anche usare  `~/hunspell`  per i file di lingua, poiché lo Snap non può accedere a  `~/.local`

- In **Windows** devi copiare i dizionari Hunspell per la tua lingua in una cartella `dicts` nella cartella dell'applicazione QOwnNotes o in `C:\Users\[username]\dicts`
    - Puoi scaricare dizionari da [dizionari LibreOffice](https://github.com/LibreOffice/dictionaries)
    - In **macOS** devi copiare i dizionari Hunspell per il tuo lingua su `/Library/Spelling` or `~/Library/Spelling`. Se la directory non esiste, è necessario crearla.

I dizionari possono essere scaricati dai [dizionari QOwnNotes](https://github.com/qownnotes/dictionaries). Utilizzare la tabella alla fine di questo documento per trovare la propria lingua e il rispettivo codice lingua.

Se il dizionario per la tua lingua non è in **Dictionary Manager** o nel collegamento sopra indicato, cercalo su Internet o crea un problema. Ci assicureremo di includerlo nella prossima versione di QOwnNotes se viene trovato il dizionario.

::: tip Info
Se non riesci a trovare la tua lingua in questa tabella, sfortunatamente non esiste un dizionario per essa nei file di LibreOffice - Se vuoi creare un dizionario, vai su [Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries)

| Sr | Language Code | Language                           |
| -- | ------------- | ---------------------------------- |
| 1  | af_ZA         | Afrikaans (South Africa)           |
| 2  | an_ES         | Aragonese - Spain (ES)             |
| 3  | ar            | Arabo                              |
| 4  | be_BY         | Bielorusso                         |
| 5  | bg_BG         | Bulgaro                            |
| 6  | bn_BD         | Bengalese                          |
| 7  | bo            | Tibetano                           |
| 8  | br_FR         | Breton - France                    |
| 9  | bs_BA         | Bosniaco                           |
| 10 | ca            | Catalano                           |
| 11 | cs_CZ         | Ceco                               |
| 12 | da_DK         | Danese                             |
| 13 | de            | Tedesco                            |
| 14 | el_GR         | Greco                              |
| 15 | en            | Inglese                            |
| 16 | es            | Spagnolo                           |
| 17 | et_EE         | Estone                             |
| 18 | gd_GB         | Gaelico                            |
| 19 | gl            | Galiziano                          |
| 20 | gu_IN         | Gujarati                           |
| 21 | gug           | Guarani                            |
| 22 | he_IL         | Ebraico                            |
| 23 | hi_IN         | Hindi                              |
| 24 | hr_HR         | Croato                             |
| 25 | id            | Indonesiano                        |
| 26 | it_IT         | Italiano                           |
| 27 | kmr_Latn      | Curdo (Turchia)                    |
| 28 | lo_LA         | Laotiano                           |
| 29 | lt_LT         | Lituano                            |
| 30 | lv_LV         | Lettone                            |
| 31 | ne_NP         | Nepalese                           |
| 32 | nl_NL         | Olandese                           |
| 33 | no            | Norvegese                          |
| 34 | oc_FR         | Occitano                           |
| 35 | pl_PL         | Polacco                            |
| 36 | pt_BR         | Portoghese brasiliano              |
| 37 | pt_PT         | Portoghese                         |
| 38 | ro            | Rumeno                             |
| 39 | ru_RU         | Russo                              |
| 40 | si_LK         | Sinhalese                          |
| 41 | sk_SK         | Slovacco                           |
| 42 | sl_Sl         | Sloveno                            |
| 43 | sq_AL         | Albanese                           |
| 44 | sr            | Serbo                              |
| 45 | sv_SE         | Svedese                            |
| 46 | sw_TZ         | Swahili                            |
| 47 | te_IN         | Tèlugu                             |
| 48 | th_TH         | Thailandese                        |
| 49 | tr_TR         | Turco                              |
| 51 | uk_UA         | Ucraino                            |
| 52 | vi            | Vietnamita                         |
| 53 | zu_ZA         | Zulu (incantesimo non disponibile) |

::: tip
Crediti [Sonnet](https://github.com/KDE/sonnet) con [Hunspell](https://hunspell.github.io/) viene utilizzato per il controllo ortografico
:::
