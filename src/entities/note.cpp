#include "entities/note.h"
#include <QDebug>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QSqlError>
#include <QRegularExpression>
#include <QUrl>
#include <QCryptographicHash>
#include "libraries/simplecrypt/simplecrypt.h"
#include "libraries/hoedown/html.h"


Note::Note() {
    this->id = 0;
    this->hasDirtyData = false;
}

int Note::getId() {
    return this->id;
}

QString Note::getName() {
    return this->name;
}

qint64 Note::getCryptoKey() {
    return this->cryptoKey;
}

QString Note::getFileName() {
    return this->fileName;
}

QString Note::getNoteText() {
    return this->noteText;
}

bool Note::getHasDirtyData() {
    return this->hasDirtyData;
}

void Note::setName(QString text) {
    this->name = text;
}

void Note::setCryptoKey(qint64 cryptoKey) {
    this->cryptoKey = cryptoKey;
}

void Note::setNoteText(QString text) {
    this->noteText = text;
}

bool Note::addNote(QString name, QString fileName, QString text) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("INSERT INTO note ( name, file_name, note_text ) VALUES ( :name, :file_name, :note_text )");
    query.bindValue(":name", name);
    query.bindValue(":file_name", fileName);
    query.bindValue(":note_text", text);
    return query.exec();
}

Note Note::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    Note note;

    query.prepare("SELECT * FROM note WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            note = noteFromQuery(query);
        }
    }

    return note;
}

Note Note::fetchByFileName(QString fileName) {
    Note note;
    note.fillByFileName(fileName);
    return note;
}

bool Note::fillByFileName(QString fileName) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("SELECT * FROM note WHERE file_name = :file_name");
    query.bindValue(":file_name", fileName);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            this->fillFromQuery(query);
            return true;
        }
    }

    return false;
}

bool Note::remove(bool withFile) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("DELETE FROM note WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        if (withFile) {
            this->removeNoteFile();
        }

        return true;
    }
}

/**
 * @brief Copies a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::copy(QString destinationPath) {
    QDir d;
    if (this->fileExists() && (d.exists(destinationPath))) {
        QFile file(fullNoteFilePath(this->fileName));
        QString destinationFileName = destinationPath + QDir::separator() + this->fileName;

        if (d.exists(destinationFileName)) {
            qDebug() << destinationFileName << "already exists!";

            // find a new filename for the note
            QDateTime currentDateTime = QDateTime::currentDateTime();
            destinationFileName = destinationPath + QDir::separator() + this->name + " " +
                                  currentDateTime.toString(Qt::ISODate).replace(":", "_") + ".txt";

            qDebug() << "New file name:" << destinationFileName;
        }

        return file.copy(destinationFileName);
    }

    return false;
}

/**
 * @brief Moves a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::move(QString destinationPath) {
    bool result = this->copy(destinationPath);

    if (result) {
        return remove(true);
    }

    return false;
}

Note Note::fetchByName(QString name) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    Note note;

    query.prepare("SELECT * FROM note WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            note = noteFromQuery(query);
        }
    }

    return note;
}

Note Note::noteFromQuery(QSqlQuery query) {
    Note note;
    note.fillFromQuery(query);
    return note;
}

bool Note::fillFromQuery(QSqlQuery query) {
    id = query.value("id").toInt();
    name = query.value("name").toString();
    fileName = query.value("file_name").toString();
    noteText = query.value("note_text").toString();
    cryptoKey = query.value("crypto_key").toLongLong();
    hasDirtyData = query.value("has_dirty_data").toInt() == 1;
    fileCreated = query.value("file_created").toDateTime();
    fileLastModified = query.value("file_last_modified").toDateTime();
    created = query.value("created").toDateTime();
    modified = query.value("modified").toDateTime();

    return true;
}

QList<Note> Note::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<Note> noteList;

    query.prepare("SELECT * FROM note ORDER BY file_last_modified DESC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteList.append(note);
        }
    }

    return noteList;
}

QList<Note> Note::search(QString text) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<Note> noteList;

    query.prepare("SELECT * FROM note WHERE note_text LIKE :text ORDER BY file_last_modified DESC");
    query.bindValue(":text", "%" + text + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteList.append(note);
        }
    }

    return noteList;
}

QList<QString> Note::searchAsNameList(QString text, bool searchInName) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<QString> nameList;
    QString searchField = searchInName ? "name" : "note_text";

    query.prepare("SELECT name FROM note WHERE " + searchField + " LIKE :text ORDER BY file_last_modified DESC");
    query.bindValue(":text", "%" + text + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value("name").toString());
        }
    }

    return nameList;
}

QStringList Note::fetchNoteNames() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QStringList list;

    query.prepare("SELECT name FROM note ORDER BY file_last_modified DESC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    }
    else {
        for (int r = 0; query.next(); r++) {
            list.append(query.value("name").toString());
        }
    }

    return list;
}

QStringList Note::fetchNoteFileNames() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QStringList list;

    query.prepare("SELECT file_name FROM note ORDER BY file_last_modified DESC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            list.append(query.value("file_name").toString());
        }
    }

    return list;
}

bool Note::storeNewText(QString text) {
    this->noteText = text;
    this->hasDirtyData = true;

    return this->store();
}

//
// inserts or updates a note object in the database
//
bool Note::store() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    if (this->fileName == "") {
        this->fileName = this->name + ".txt";
    }

    if (this->id > 0) {
        query.prepare("UPDATE note SET "
                              "name = :name,"
                              "file_name = :file_name,"
                              "note_text = :note_text,"
                              "has_dirty_data = :has_dirty_data, "
                              "file_last_modified = :file_last_modified,"
                              "file_created = :file_created,"
                              "crypto_key = :crypto_key,"
                              "modified = :modified "
                              "WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare("INSERT INTO note"
                              "( name, file_name, note_text, has_dirty_data,"
                              "file_last_modified, file_created, crypto_key,"
                              "modified ) "
                              "VALUES ( :name, :file_name, :note_text,"
                              ":has_dirty_data, :file_last_modified,"
                              ":file_created, :crypto_key, :modified )");
    }

    QDateTime modified = QDateTime::currentDateTime();

    query.bindValue(":name", this->name);
    query.bindValue(":file_name", this->fileName);
    query.bindValue(":note_text", this->noteText);
    query.bindValue(":has_dirty_data", this->hasDirtyData ? 1 : 0);
    query.bindValue(":file_created", this->fileCreated);
    query.bindValue(":file_last_modified", this->fileLastModified);
    query.bindValue(":crypto_key", this->cryptoKey);
    query.bindValue(":modified", modified);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (this->id == 0) {  // on insert
        this->id = query.lastInsertId().toInt();
    }

    this->modified = modified;
    return true;
}

bool Note::storeNoteTextFileToDisk() {
    // checks if filename has to be changed (and change it if needed)
    this->handleNoteTextFileName();

    QFile file(fullNoteFilePath(this->fileName));
    bool fileExists = this->fileExists();

    qDebug() << "storing note file: " << this->fileName;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << file.errorString();
        return false;
    }

    // transform all types of newline to \n
    // (maybe the ownCloud-sync works better then)
    QString text = this->noteText;
    text.replace(QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"), "\n");

//    diff_match_patch *diff = new diff_match_patch();
//    QList<Diff> diffList = diff->diff_main( this->noteText, text );

//    QString html = diff->diff_prettyHtml( diffList );
//    diff->diff_cleanupSemantic( diffList );
//    qDebug() << __func__ << " - 'diffList': " << diffList[0].toString();
//    qDebug() << __func__ << " - 'html': " << html;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << text;
    file.flush();
    file.close();

    this->hasDirtyData = false;
    this->fileLastModified = QDateTime::currentDateTime();

    if (!fileExists) {
        this->fileCreated = this->fileLastModified;
    }

    return this->store();
}

//
// checks if filename has to be changed
// generates a new name and filename and removes the old file
// (the new file is not stored to a note text file!)
//
void Note::handleNoteTextFileName() {
    // split the text into a string list
    QStringList noteTextLines = this->noteText.split(
            QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"));

    // do nothing if there is no text
    if (noteTextLines.count() == 0) return;

    QString name = noteTextLines[0];
    // do nothing if the first line is empty
    if (name == "") return;

    // check if name has changed
    if (name != this->name) {
        qDebug() << __func__ << " - 'name' was changed: " << name;
        QString fileName = name + ".txt";

        // check if note with this filename already exists
        Note note = Note::fetchByFileName(fileName);
        if (note.id > 0) {
            // find new filename for the note (not very safe yet)
            QDateTime currentDateTime = QDateTime::currentDateTime();
            name += " " + currentDateTime.toString(Qt::ISODate).replace(":", "_");
        }

        // remove old note file
        // TODO: note doesn't seem to be removed very often
        this->removeNoteFile();

        // update first line of note text
        // TODO: UI has to be updated too then!
        noteTextLines[0] = name;
        this->noteText = noteTextLines.join("\n");

        // store new name and filename
        this->name = name;
        this->fileName = name + ".txt";
        this->store();
    }

    qDebug() << __func__ << " - 'name': " << name;
    qDebug() << __func__ << " - 'this->id': " << this->id;
}

bool Note::updateNoteTextFromDisk() {
    QFile file(fullNoteFilePath(this->fileName));

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    this->noteText = in.readAll();
    file.close();

    // strangely it sometimes gets null
    if (this->noteText.isNull()) this->noteText = "";

    return true;
}

QString Note::fullNoteFilePath(QString fileName) {
    QSettings settings;
    QString notesPath = settings.value("notesPath").toString();

    return notesPath + QDir::separator() + fileName;
}

int Note::storeDirtyNotesToDisk(Note &currentNote) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    Note note;
//    qDebug() << "storeDirtyNotesToDisk";

    query.prepare("SELECT * FROM note WHERE has_dirty_data = 1");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return 0;
    } else {
        int count = 0;
        for (int r = 0; query.next(); r++) {
            note = noteFromQuery(query);
            QString oldFileName = note.getFileName();
            note.storeNoteTextFileToDisk();
            QString newFileName = note.getFileName();

            // reassign currentNote if filename of currentNote has changed
            if ((oldFileName == currentNote.getFileName()) && (oldFileName != newFileName)) {
                currentNote = note;
                qDebug() << "filename of currentNote has changed to: " << newFileName;
            }

            qDebug() << "stored note: " << note;
            count++;
        }

        return count;
    }
}

void Note::createFromFile(QFile &file) {
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");

        // qDebug() << file.size() << in.readAll();
        QString noteText = in.readAll();
        file.close();

        QFileInfo fileInfo;
        fileInfo.setFile(file);

        // create a nicer name by removing ".txt"
        // TODO: make sure name is ownCloud Notes conform
        QString name = fileInfo.fileName();
        name.chop(4);

        this->name = name;
        this->fileName = fileInfo.fileName();
        this->noteText = noteText;
        this->fileCreated = fileInfo.created();
        this->fileLastModified = fileInfo.lastModified();
        this->store();
    }

}

//
// deletes all notes in the database
//
bool Note::deleteAll() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare("DELETE FROM note");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

//
// checks if file of note exists in the filesystem
//
bool Note::fileExists() {
    QFile file(fullNoteFilePath(this->fileName));
    return file.exists();
}

//
// checks if the current note still exists in the database
//
bool Note::exists() {
    Note note = Note::fetch(this->id);
    return note.id > 0;
}

//
// reloads the current Note (by fileName)
//
bool Note::refetch() {
    return this->fillByFileName(this->fileName);
}

//
// remove the file of the note
//
bool Note::removeNoteFile() {
    if (this->fileExists()) {
        QFile file(fullNoteFilePath(this->fileName));
        qDebug() << __func__ << " - 'this->fileName': " << this->fileName;
        qDebug() << __func__ << " - 'file': " << file.fileName();
        return file.remove();
    }

    return false;
}

/**
 * @brief Returns html rendered markdown of the note text
 * @param notesPath for transforming relative local urls to absolute ones
 * @return
 */
QString Note::toMarkdownHtml(QString notesPath) {
    hoedown_renderer *renderer =
            hoedown_html_renderer_new(HOEDOWN_HTML_USE_XHTML, 16);
    hoedown_extensions extensions =
            (hoedown_extensions) (HOEDOWN_EXT_BLOCK | HOEDOWN_EXT_SPAN);
    hoedown_document *document = hoedown_document_new(renderer, extensions, 16);

    // get the decrypted note text (or the normal note text if there isn't any)
    QString str = getDecryptedNoteText();

    // parse for relative file urls and make them absolute
    // (for example to show images under the note path)
    str.replace(
            QRegularExpression("\\(file:\\/\\/([^\\/].+)\\)"),
            "(file://" + notesPath + "/\\1)");

    unsigned char *sequence = (unsigned char *) qstrdup(str.toUtf8().constData());
    int length = strlen((char *) sequence);

    // return an empty string if the note is empty
    if (length == 0) {
        return "";
    }

    hoedown_buffer *html = hoedown_buffer_new(length);

    // render markdown html
    hoedown_document_render(document, html, sequence, length);

    // get markdown html
    QString result = QString::fromUtf8((char *) html->data, html->size);

    /* Cleanup */
    free(sequence);
    hoedown_buffer_free(html);

    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    result =
            "<html><head><style>h1, h2, h3 { margin: 5pt 0 10pt 0; }"
            "a { color: #FF9137; text-decoration: none; }</style></head><body>" +
            result + "</body></html>";

    // check if width of embedded local images is too high
    QRegularExpression re("<img src=\"file:\\/\\/([^\"]+)\"");
    QRegularExpressionMatchIterator i = re.globalMatch(result);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString fileName = match.captured(1);
        QImage image(fileName);

        // cap the image width at 980px
        if (image.width() > 980) {
            result.replace(
                    QRegularExpression("<img src=\"file:\\/\\/" + QRegularExpression::escape(fileName) + "\""),
                           "<img width=\"980\" src=\"file://" + fileName + "\"");
        }
    }

    return result;
}

bool Note::isFetched() {
    return (this->id > 0);
}

/**
 * @brief Generates a text that can be used in a link
 * @param text
 * @return
 */
QString Note::generateTextForLink(QString text) {
    // replace everything but characters and numbers with "_"
    QRegularExpression re(
            "[^\\d\\w]", QRegularExpression::CaseInsensitiveOption);
    text.replace(re, "_");
    return text;
}

/**
 * Generates a qint64 hash from a QString
 */
qint64 Note::qint64Hash(const QString & str) {
    QByteArray hash = QCryptographicHash::hash(
            QByteArray::fromRawData((const char*)str.utf16(), str.length()*2),
            QCryptographicHash::Md5);
    Q_ASSERT(hash.size() == 16);
    QDataStream stream(hash);
    qint64 a, b;
    stream >> a >> b;
    return a ^ b;
}

/**
 * Encrypts the note text with the note's crypto key
 */
QString Note::encryptNoteText() {
    // split the text into a string list
    QStringList noteTextLines = this->noteText.split(
            QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"));

    // keep the first two lines unencrypted
    noteText = noteTextLines.at(0) + "\n" + noteTextLines.at(1) + "\n\n" +
            QString(NOTE_TEXT_ENCRYPTION_PRE_STRING) + "\n";

    // remove the first two lines for encryption
    noteTextLines.removeFirst();
    noteTextLines.removeFirst();

    // remove the 3rd line too if it is empty
    if (noteTextLines.at(0) == "") {
        noteTextLines.removeFirst();
    }

    // join the remaining lines
    QString text = noteTextLines.join("\n");

    // empty notes will be detected as "can't be decrypted",
    // so we will add a space
    if (text.isEmpty()) {
        text = " ";
    }

    // encrypt the text
    SimpleCrypt* crypto = new SimpleCrypt(static_cast<quint64>(cryptoKey));
    QString encryptedText = crypto->encryptToString(text);

    // add the encrypted text to the new note text
    noteText += encryptedText + "\n" +
            QString(NOTE_TEXT_ENCRYPTION_POST_STRING);

    // store note
    store();

    return noteText;
}

/**
 * Returns the regular expression to match encrypted text
 */
QRegularExpression Note::getEncryptedNoteTextRegularExpression() {
    // match the encrypted string
    QRegularExpression re(
            QRegularExpression::escape(NOTE_TEXT_ENCRYPTION_PRE_STRING) +
            "\\s+(.+)\\s+" +
            QRegularExpression::escape(NOTE_TEXT_ENCRYPTION_POST_STRING));

    re.setPatternOptions(
            QRegularExpression::MultilineOption |
            QRegularExpression::DotMatchesEverythingOption);

    return re;
}

/**
 * Returns encrypted note text if it is encrypted
 */
QString Note::getEncryptedNoteText() {
    QString noteText = this->noteText;

    // get regular expression for the encrypted string
    QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // check if we have an encrypted note text and return it if so
    QRegularExpressionMatch match = re.match(noteText);
    return match.hasMatch() ? match.captured(1) : "";
}

/**
 * Returns encrypted note text if it is encrypted
 */
bool Note::hasEncryptedNoteText() {
    return !getEncryptedNoteText().isEmpty();
}

/**
 * Checks if note text can be decrypted
 */
bool Note::canDecryptNoteText() {
    QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText == "") {
        return false;
    }

    // decrypt the note text
    SimpleCrypt* crypto = new SimpleCrypt(static_cast<quint64>(cryptoKey));
    QString decryptedNoteText = crypto->decryptToString(encryptedNoteText);

    return decryptedNoteText != "";
}

/**
 * Sets the password to generate the cryptoKey
 */
void Note::setCryptoPassword(QString password) {
    cryptoKey = qint64Hash(password);
}

/**
 * Returns decrypted note text if it is encrypted
 * The crypto key has to be set in the object
 */
QString Note::getDecryptedNoteText() {
    QString noteText = this->noteText;
    QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText == "") {
        return noteText;
    }

    // decrypt the note text
    SimpleCrypt* crypto = new SimpleCrypt(static_cast<quint64>(cryptoKey));
    QString decryptedNoteText = crypto->decryptToString(encryptedNoteText);

    if (decryptedNoteText == "") {
        return noteText;
    }

    // get regular expression for the encrypted string
    QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // replace the encrypted text with the decrypted text
    noteText.replace(re, decryptedNoteText);
    return noteText;
}

QDebug operator<<(QDebug dbg, const Note &note) {
    dbg.nospace() << "Note: <id>" << note.id << " <name>" << note.name <<
            " <fileName>" << note.fileName <<
            " <hasDirtyData>" << note.hasDirtyData;
    return dbg.space();
}
