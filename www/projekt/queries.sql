-- Podstawa X Skladniki X Zamowiona
-- Wypisywanie nazw, cen i skladnikow pizzy
-- SELECT pdp.nazwa, pdp.cena, group_concat(sk.nazwa) AS 'składniki'
--     FROM addClient_podstawadopizzy pdp 
--     LEFT JOIN addClient_skladnikipodstawy sp ON pdp.id = sp.podstawa_id
--     LEFT JOIN addClient_skladnik sk ON sp.skladnik_id = sk.id
--     GROUP BY pdp.nazwa;


-- addClient_klient kl, addClient_zamowienie zam, addClient_zamowionapizza zp,
-- addClient_podstawadopizzy pdp, addClient_dodatkoweskladniki ds, addClient_rozmiar roz,
-- addClient_skladnikipodstawy sp, addClient_skladnik sk
-- Klient X Zamowienie X ZamowionaPizza X Podstawa X Rozmiar X DodatkoweSkladniki X Skladnik
-- Wypisywanie zamówień klienta z rozmiarem


.width 300
-- grupowanie pizz
--zam.data, zam.godzina
-- SELECT zam.id, kl.imie, kl.nazwisko, kl.telefon, roz.nazwa, 
--         pdp.nazwa || IFNULL("(+" || GROUP_CONCAT(sk.nazwa, ',') || ")", '') as 'pizza', 
--         (pdp.cena + SUM(cena_domyslna)) * roz.mnoznik_ceny as 'cena'
--     FROM addClient_zamowienie zam
--     LEFT JOIN addClient_klient kl ON zam.klient_id = kl.id
--     LEFT JOIN addClient_zamowionapizza zp ON zp.zamowienie_id = zam.id
--     LEFT JOIN addClient_podstawadopizzy pdp ON zp.podstawa_id = pdp.id
--     LEFT JOIN addClient_rozmiar roz ON zp.rozmiar_id = roz.id
--     LEFT JOIN addClient_dodatkoweskladniki ds ON ds.zamowiona_id = zp.id
--     LEFT JOIN addClient_skladnik sk ON ds.skladnik_id = sk.id
--     GROUP BY zp.id;

-- grupowanie zamowien
SELECT z.id, kl.imie, kl.nazwisko, kl.telefon, z.data, z.godzina,
    GROUP_CONCAT(z.pizza || ' za ' || z.cena || ' zl', ',') as zamowione,  
    PRINTF("%.2f", SUM(z.cena)) || ' zl' as cena
    FROM (
        SELECT zam.id as id,
            zam.data as data,
            zam.godzina as godzina, 
            zam.klient_id as klid,
            roz.nazwa || " " || pdp.nazwa || IFNULL("(+" || GROUP_CONCAT(sk.nazwa, ',') || ")", '') as pizza, 
            PRINTF("%.2f", (pdp.cena + IFNULL(SUM(cena_domyslna),0)) * roz.mnoznik_ceny) as cena
        FROM addClient_zamowienie zam
        LEFT JOIN addClient_zamowionapizza zp ON zp.zamowienie_id = zam.id
        LEFT JOIN addClient_podstawadopizzy pdp ON zp.podstawa_id = pdp.id
        LEFT JOIN addClient_rozmiar roz ON zp.rozmiar_id = roz.id
        LEFT JOIN addClient_dodatkoweskladniki ds ON ds.zamowiona_id = zp.id
        LEFT JOIN addClient_skladnik sk ON ds.skladnik_id = sk.id
        GROUP BY zp.id) z
    LEFT JOIN addClient_klient kl ON z.klid = kl.id
    GROUP BY z.id;