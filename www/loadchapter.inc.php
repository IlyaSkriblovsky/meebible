<?php

function loadChapterXML($trans, $lang, $book, $chapter) {
    if ($trans != 'nwt')
    {
        $db = new SQLite3("content/$trans.sqlite");
        if (! $db) return null;

        $stmt = $db->prepare("SELECT html FROM html WHERE langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");
        if (! $stmt)
        {
            $db->close();
            return null;
        }

        $stmt->reset();
        $stmt->bindValue(':langCode', $lang);
        $stmt->bindValue(':bookCode', $book);
        $stmt->bindValue(':chapterNo', $chapter, SQLITE3_INTEGER);
        $r = $stmt->execute();
        $row = $r->fetchArray();
        if (empty($row))
        {
            $db->close();
            return null;
        }

        $html = $row[0];

        $db->close();

        return $html;
    }
    else
    {
        $filename = "content/nwt/nwt_$lang.sqlite";
        if (file_exists($filename))
        {
            $db = new SQLite3($filename);
            if (! $db) return null;

            $stmt = $db->prepare("SELECT html FROM html WHERE langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");
            if (! $stmt)
            {
                $db->close();
                return null;
            }

            $stmt->reset();
            $stmt->bindValue(':langCode', $lang);
            $stmt->bindValue(':bookCode', $book);
            $stmt->bindValue(':chapterNo', $chapter, SQLITE3_INTEGER);

            $r = $stmt->execute();
            $row = $r->fetchArray();
            if (empty($row))
            {
                $db->close();
                return null;
            }

            $html = $row[0];

            $db->close();

            return $html;
        }
        else
            return null;
    }
}


function bookName($trans, $lang, $book) {
    $db = new SQLite3("content/trans.sqlite");
    if (! $db) return null;

    $stmt = $db->prepare("SELECT bookName FROM books WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode");
    if (! $stmt)
    {
        $db->close();
        return null;
    }

    $stmt->reset();
    $stmt->bindValue(':transCode', $trans);
    $stmt->bindValue(':langCode', $lang);
    $stmt->bindValue(':bookCode', $book);
    $r = $stmt->execute();
    $row = $r->fetchArray();
    if (empty($row))
    {
        $db->close();
        return null;
    }

    $name = $row[0];

    $db->close();

    return $name;
}

?>
