package main

import (
	"fmt"
	"math/rand"
	"sort"
	"time"
)

const WordSize = 32

type entity = uint32

type chromosome struct {
	genes entity
	score int
}

func (ch *chromosome) inspect() string {
	return fmt.Sprintf("genes:%032b\tscore:%d", ch.genes, ch.score)
}

func (ch *chromosome) eval(fn func(entity) int) {
	ch.score = fn(ch.genes)
}

func (ch *chromosome) mutate(position uint8) {
	ch.genes ^= 1 << (position % WordSize)
}

func (ch *chromosome) halfGenes() entity {
	return ((1 << (WordSize/2 + 1)) - 1) & ch.genes
}

func (ch *chromosome) cross(partner chromosome) chromosome {
	genes := (ch.halfGenes() << (WordSize / 2)) | partner.halfGenes()
	return chromosome{genes: genes}
}

type population []chromosome

func (p population) Len() int {
	return len(p)
}

func (p population) Swap(i, j int) {
	p[i], p[j] = p[j], p[i]
}

func (p population) Less(i, j int) bool {
	return p[i].score < p[j].score
}

type generation struct {
	population population
}

func (g *generation) eval(fn func(entity) int) {
	for idx := range g.population {
		g.population[idx].eval(fn)
	}
}

func (g *generation) inspect() string {
	var ret string
	for idx, chromosome := range g.population {
		ret += fmt.Sprintf("idx:%02d\t%s\n", idx, chromosome.inspect())
	}
	return ret
}

func (g *generation) selectChromosomes(c int) []chromosome {
	dup := make(population, len(g.population))
	copy(dup, g.population)
	sort.Sort(sort.Reverse(dup))
	return dup[:c]
}

func (g *generation) crossChromosomes(c int) []chromosome {
	ret := make(population, c)
	for i := 0; i < c; i++ {
		idx := rand.Intn(len(g.population) - 1)
		ret[i] = g.population[idx].cross(g.population[idx+1])
	}
	return ret
}

func (g *generation) mutateChromosomes(c int) []chromosome {
	ret := make(population, c)
	for i := 0; i < c; i++ {
		idx := rand.Intn(len(g.population) - 1)
		ret[i] = g.population[idx]
	}
	for idx := range ret {
		ret[idx].mutate(uint8(rand.Intn(WordSize)))
	}
	return ret
}

func (g *generation) evolve(r recipe) *generation {
	population := []chromosome{}

	population = append(population, g.selectChromosomes(r.selection)...)
	population = append(population, g.crossChromosomes(r.crossover)...)
	population = append(population, g.mutateChromosomes(r.mutation)...)

	return &generation{population: population}
}

type recipe struct {
	selection int
	crossover int
	mutation  int
}

func performGA(firstGeneration *generation, fitnessFunction func(entity) int, recipe recipe, iteration int) {
	var currentGen *generation
	currentGen = firstGeneration
	currentGen.eval(fitnessFunction)
	for i := 0; i < iteration; i++ {
		inspect := currentGen.inspect()
		fmt.Printf("----- %dth generation ----\n%s", i, inspect)
		currentGen = currentGen.evolve(recipe)
		currentGen.eval(fitnessFunction)
	}
	inspect := currentGen.inspect()
	fmt.Printf("----- result ----\n%s", inspect)
}

func main() {
	rand.Seed(time.Now().UnixNano())

	base := time.Now()

	firstGen := generation{
		population: []chromosome{
			chromosome{genes: 0x00000000},
			chromosome{genes: 0x00000000},
			chromosome{genes: 0x00000000},
			chromosome{genes: 0x00000001},
			chromosome{genes: 0x00000010},
			chromosome{genes: 0x00000100},
			chromosome{genes: 0x00001000},
			chromosome{genes: 0x00010000},
			chromosome{genes: 0x00100000},
			chromosome{genes: 0x01000000},
			chromosome{genes: 0x10000000},
		},
	}

	fn := func(genes entity) int {
		var ret = 0
		for i := 0; i < 32; i++ {
			if (genes & (1 << uint8(i))) > 0 {
				ret++
			}
		}
		return ret
	}

	r := recipe{selection: 10, crossover: 50, mutation: 50}
	N := 10000
	performGA(&firstGen, fn, r, N)

	fmt.Println(time.Since(base))
}
