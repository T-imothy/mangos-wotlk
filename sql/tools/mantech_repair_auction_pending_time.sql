-- One-time repair for auctions created with an uninitialized moneyDeliveryTime.
-- Run against the CHARACTER database with the world server stopped.
-- Only unsold auctions whose items still exist outside mail are eligible.
-- Deploy the AuctionEntry default-initialization fix before restarting.
UPDATE auction a
JOIN item_instance i ON i.guid = a.itemguid
LEFT JOIN mail_items m ON m.item_guid = a.itemguid
SET a.moneyTime = 0
WHERE a.moneyTime <> 0 AND a.itemguid <> 0
  AND a.buyguid = 0 AND a.lastbid = 0
  AND m.item_guid IS NULL;
