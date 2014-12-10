import os

VOS_DOS             = 0x00010000
VOS_NT              = 0x00040000
VOS__WINDOWS32      = 0x00000004
VOS_DOS_WINDOWS16   = 0x00010001
VOS_DOS_WINDOWS32   = 0x00010004
VOS_NT_WINDOWS32    = 0x00040004

def normalizer(s):
    for j in range(len(s)):
        if len(s[j]) > 3:
            k = s[j][2:]
        else:
            k = '0' + s[j][2:]
        s[j] = k
    return s

def CalcVersionInfo(fn):
    ostypes = [VOS_DOS, VOS_NT, VOS__WINDOWS32, VOS_DOS_WINDOWS16,
               VOS_DOS_WINDOWS32, VOS_NT_WINDOWS32]

    verstrings = []
    sigstrings = findsignatures(fn)
    if sigstrings[0] == '':
        print('No Version Information Available')
        return
    for i in sigstrings:
        FV = normalizer(i.split(',')[8:16])
        FOS = normalizer(i.split(',')[32:36])
        hexver = FV[3]+FV[2]+FV[1]+FV[0]+':'+FV[7]+FV[6]+FV[5]+FV[4]
        OStag = int('0x' + FOS[3]+FOS[2]+FOS[1]+FOS[0], 16)
        if OStag not in ostypes:
           continue
        if hexver not in verstrings:
           verstrings.append(hexver)
    myver = max(verstrings)
    return parsver(myver)

def createparsestruct(b):
    s = ''
    for i in range(len(b)):
        s += hex(b[i])+','
    return s[:-1]

def findsignatures(file):
    f = open(file, 'rb')
    sz = f.read()
    f.close()
    res = []
    indx=sz.find(b'\xbd\x04\xef\xfe')
    cnt = sz.count(b'\xbd\x04\xef\xfe')
    while cnt > 1:
        s = createparsestruct(sz[indx:indx+52])
        sz = sz[indx+1:]
        cnt = sz.count(b'\xbd\x04\xef\xfe')
        indx=sz.find(b'\xbd\x04\xef\xfe')
        res.append(s)
    res.append(createparsestruct(sz[indx:indx+52]))
    return res

def parsver(v):
    a,b,c,d = v[:4], v[4:8], v[9:13], v[13:]
    return str(int(a,16)) + '.'+ str(int(b,16)) +'.' + str(int(c,16)) + '.' + str(int(d,16))
