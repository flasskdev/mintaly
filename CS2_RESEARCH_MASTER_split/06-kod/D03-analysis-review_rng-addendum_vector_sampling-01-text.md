<!-- split-kod | 04-otchety/D03-analysis-review_rng-addendum_vector_sampling.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D03-analysis-review_rng-addendum_vector_sampling.md` (язык `text`)

[← исходная часть](../04-otchety/D03-analysis-review_rng-addendum_vector_sampling.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D03-analysis-review_rng-addendum_vector_sampling.md`; в самой части блок остаётся на месте. Символов: 168.

````text
y = a*x[i],  a=0.939413070679 ≈ exp(-8*h)
D(x[i]) = |y|>18*h ? y*(1−18*h/|y|) : 0
v[i] = F(-4.5*i*h)*v0;  при |v[i]|²<1/1024 → 0
x[i+1] = D(x[i]) + (h/2)*(v[i]+v[i+1])
````
