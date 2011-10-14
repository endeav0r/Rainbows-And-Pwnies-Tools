import sys

fh = open('api/header.html', 'r')
HEADER = fh.read()
fh.close()

fh = open('api/footer.html', 'r')
FOOTER = fh.read()
fh.close()

fh = open(sys.argv[1], 'r')
raw = fh.read()
fh.close()

lines = raw.split('\n')

items = []
item = {}
i = 0
for line in lines :
    if line == '' :
        items.append(item)
        item = {}
    elif line[0] == '@' :
        pieces = line.split(' ')
        
        if pieces[0] == '@return' :
            types = pieces[1].split(',')
            description = ' '.join(pieces[2:])
            item['return'] = {"types": types, "description": description}
            
        elif pieces[0] == '@param' :
            if not item.has_key('params') :
                item['params'] = []
            types = pieces[1].split(',')
            description = ' '.join(pieces[2:])
            item['params'].append({"types": types, "description": description})
    else :
        item['title'] = line

print HEADER

for item in items :
    if item.has_key('params') :
        params = ','.join(map(lambda x: ' | '.join(x["types"]), item["params"]))
    else :
        params = ''
    print '<div class="function">'
    print '<h3>' + item["title"] + "(" + params + ")" + '</h3>'
    print '<table>'
    if item.has_key('params') :
        for param in item["params"] :
            print '<tr>'
            print '<td class="bg1"><strong>@param</strong></td>'
            print '<td class="bg2"><strong>' + ' | '.join(param["types"]) + '</strong></td>'
            print '<td class="bg1"><em>' + param["description"] + '</em></td>'
            print '</tr>'
    print '<tr>'
    print '<td class="bg1"><strong>@return</strong></td>'
    print '<td class="bg2"><strong>' + ' | '.join(item["return"]["types"]) + '</strong></td>'
    print '<td class="bg1"><em>' + item["return"]["description"] + '</em></td>'
    print '</tr></table>'
    print '</div><br />'
    
print FOOTER
