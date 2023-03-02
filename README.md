# misskey_tool
something to misskey...?

## フューチャーのフィーチャー
 - 芋づる式にMisskeyインスタンスを探す
 - それ以外は考え中

## required
this need these lib/package.
 - cpprestsdk
 - libpqxx
 - nlohmann/json
 - redis++

## database

### redis
| key| value |   
|:--:|:--:|
| misskey_tool:queue:example.com | "0" |    
| misskey_tool:history:example.com | "" |
 -  "misskey_tool:queue:*" は、値に前回取得した時の連合数を入れる（初回なら0）