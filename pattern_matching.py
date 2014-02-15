#coding=utf-8

def Naive(T, W):
    i, j = 0, 0
    pos = []
    while i < len(T) and j < len(W):
        if T[i] == W[j]:
            j += 1
            if j == len(W):
                head = i - j + 1
                pos.append(head)
                j = 0
        else:
            j = 0

        i += 1

    return pos

def KnuthMorrisPratt(T, W):
    pass

T = u"""力まかせのアルゴリズムは古くから知られ，広く使われていました(もちろん現在でも広く使われています)。このアルゴリズムは，最悪の場合，計算量がO(mn)になりますが，通常はO(n)で実行することができます。そのために，この分野では長期にわたって，力まかせ法が唯一のアルゴリズムとして知 られていました。しかし1970年に，S.A.Cookが「最悪の場合でも文字の比較をm+nに比例した回数だけ行う文字列探索アルゴリズムが存在する」ということを理論的に証明しました。そして，この証明をもとに，D.E.KnuthとV.R.Prattの2人が，実用になる文字列探索アルゴリズムを得ることに成功しました。また，彼らとは独立して，J.H.Morrisがテキストエディタを作成する過程でほぼ同じアルゴリズムを発見しています。このアルゴリズムは3人(Knuth，Morris，Pratt)の名前をとってKMP法と呼ばれます。KMP法の計算量はO(n)になります。"""

print Naive(T, u"Knuth")
print Naive(T, u"Pratt")
print Naive(T, u"アルゴリズム")

print KnuthMorrisPratt(T, u"Knuth")
print KnuthMorrisPratt(T, u"Pratt")
print KnuthMorrisPratt(T, u"アルゴリズム")
