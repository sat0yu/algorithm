#coding=utf-8

import urllib
import urllib2
import sys
import math
from BeautifulSoup import BeautifulSoup

appid="5mAjQ8axg67Wek4h6p0.QQ26PlbsM4nh7WVpkhfdAd9b1ULrCciyxar5mdWxvwTVGVD77g--"
apiurl="http://jlp.yahooapis.jp/MAService/V1/parse"

def split(sentence):
    #sentence = sentence.encode('utf-8')
    params = urllib.urlencode({
            'appid':appid,
            'results': 'ma',
            'sentence': sentence,
            'filter': '1|2|3|4|5|9|10'})
    results = urllib2.urlopen(apiurl, params)
    soup = BeautifulSoup(results.read())
    return [word.surface.string for word in soup.ma_result.word_list]

class BayesFilter():
    def __init__(self):
        self.vocablaries = set() # 集合
        self.wordcount = {}
        self.catcount = {}

    def countupcat(self, cat):
        # 存在しないキーを指定するとエラー
        # setdefaultメソッドで初期化
        self.catcount.setdefault(cat, 0)
        self.catcount[cat] += 1

    # 指定されたカテゴリにおける、単語の登場数
    def countupword(self, word, cat):
        self.wordcount.setdefault(cat, {})
        self.wordcount[cat].setdefault(word, 0)
        self.wordcount[cat][word] += 1
        self.vocablaries.add(word) # 集合なので、まだ存在しなければ追加

    def train(self, sentence, cat):
        words = split(sentence)
        for word in words:
            self.countupword(word, cat)
        self.countupcat(cat)

    # P(cat)
    def priorprob(self, cat):
        ncats = self.catcount.values() # 各カテゴリーの登場数（訓練数）
        return float(self.catcount[cat]) / sum(ncats)

    # あるカテゴリにおける指定した単語の登場回数
    def nincategory(self, word, cat):
        if word in self.wordcount[cat]:
            return self.wordcount[cat][word]
        return 0.0

    # P(word|cat)を、(あるカテゴリで出現した回数)/（あるカテゴリーに登録された単語の総数）
    def wordprob(self, word, cat):
        # 定数1.0を加算してスムージング(ラプラス法)
        return (self.nincategory(word, cat)+1.0) / (sum(self.wordcount[cat].values()) + len(self.vocablaries) * 1.0)

    # カテゴリごとの比較を目的としてるのでスコアは対数スケールでOK
    def score(self, words, cat):
        score = math.log(self.priorprob(cat))
        for word in words:
            score += math.log(self.wordprob(word, cat))
        return score

    def classifier(self, sentence):
        best = None # 初期値
        max = -sys.maxint # 初期値
        words = split(sentence) # 予め形態素に分解

        # catcountに登録されているすべてのカテゴリを走査
        for cat in self.catcount.keys():
            score = self.score(words, cat)
            # もし現在の最高確率より高い場合は変更
            if score > max:
                max = score
                best = cat

        return best

if __name__ == "__main__":
    nb = BayesFilter()

    nb.train('''Python（パイソン）は，オランダ人のグイド・ヴァンロッサムが作ったオープンソースのプログラミング言語。
オブジェクト指向スクリプト言語の一種であり，Perlとともに欧米で広く普及している。
イギリスのテレビ局 BBC が製作したコメディ番組『空飛ぶモンティパイソン』にちなんで名付けられた。
Python は英語で爬虫類のニシキヘビの意味で，Python言語のマスコットやアイコンとして使われることがある。Pythonは汎用の高水準言語である。
プログラマの生産性とコードの信頼性を重視して設計されており，核となるシンタックスおよびセマンティクスは必要最小限に抑えられている反面，利便性の高い大規模な標準ライブラリを備えている。
Unicode による文字列操作をサポートしており，日本語処理も標準で可能である。多くのプラットフォームをサポートしており（動作するプラットフォーム），また，豊富なドキュメント，豊富なライブラリがあることから，産業界でも利用が増えつつある。''', 'Python')

    nb.train('''Ruby（ルビー）は，まつもとゆきひろ（通称Matz）により開発されたオブジェクト指向スクリプト言語であり，従来 Perlなどのスクリプト言語が用いられてきた領域でのオブジェクト指向プログラミングを実現する。
Rubyは当初1993年2月24日に生まれ， 1995年12月にfj上で発表された。名称のRubyは，プログラミング言語Perlが6月の誕生石であるPearl（真珠）と同じ発音をすることから，まつもとの同僚の誕生石（7月）のルビーを取って名付けられた。''', 'Ruby')

    nb.train('''豊富な機械学習（きかいがくしゅう，Machine learning）とは，人工知能における研究課題の一つで，人間が自然に行っている学習能力と同様の機能をコンピュータで実現させるための技術・手法のことである。
ある程度の数のサンプルデータ集合を対象に解析を行い，そのデータから有用な規則，ルール，知識表現，判断基準などを抽出する。データ集合を解析するため，統計学との関連も非常に深い。
機械学習は検索エンジン，医療診断，スパムメールの検出，金融市場の予測，DNA配列の分類，音声認識や文字認識などのパターン認識，ゲーム戦略，ロボット，など幅広い分野で用いられている。応用分野の特性に応じて学習手法も適切に選択する必要があり，様々な手法が提案されている。
それらの手法は， Machine Learning や IEEE Transactions on Pattern Analysis and Machine Intelligence などの学術雑誌などで発表されることが多い。''', '機械学習')

    words = 'ヴァンロッサム氏によって開発されました.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))

    words = '豊富なドキュメントや豊富なライブラリがあります.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))

    words = '純粋なオブジェクト指向言語です.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))

    words = 'Rubyはまつもとゆきひろ氏(通称Matz)により開発されました.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))

    words = '「機械学習」はじめようが始まりました.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))

    words = '検索エンジンや画像認識に利用されています.'
    print '%s => 推定カテゴリ: %s' % (words ,nb.classifier(words))
