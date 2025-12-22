# Trabalho Prático II- Manipulação e Reorganizao de Arquivos de Dados

Este projeto simula operações de manutenção em arquivos de dados estruturados em blocos, abordando problemas como fragmentação interna e externa, desperdício de espaço e perda de desempenho.
O arquivo alunos.dat é utilizado para armazenar registros de forma persistente, permitindo implementar inserção, exclusão lógica, edição, reaproveitamento de espaço livre, reorganização física e relatórios de eficiência.

Objetivos
- Implementar inserção, edição e exclusão de registros.
- Controlar o reaproveitamento de espaços livres.
- Respeitar o tamanho máximo de bloco definido pelo usuário.
- Permitir inclusão de registros em blocos parcialmente ocupados ou no final do arquivo.
- Implementar reorganização física para eliminar fragmentações.
- Gerar relatórios comparativos de eficiência antes e depois da reorganização.

Metodologia
- Geração de registros fictícios para testes em larga escala.
- Estrutura de dados com campos: matrícula, nome, CPF, curso, filiação, ano de ingresso e coeficiente acadêmico.
- Tipos de registros:
- Fixos: ocupam sempre o mesmo espaço.
- Variáveis: tamanho depende do conteúdo textual.
- Metadados armazenam ocupação de cada bloco (ID e bytes utilizados).

Operações Implementadas

- Inserção: aproveita espaços de registros removidos; caso contrário, insere no final.
- Edição: altera campos; se o novo tamanho exceder o original, realoca o registro.
- Exclusão lógica: marca registro como removido sem apagá-lo fisicamente.
- Listagem: exibe apenas registros ativos.
- Mapa de ocupação: mostra blocos, registros ativos/removidos, bytes ocupados/livres e eficiência.
- Reorganização física: gera alunos_reorg.dat com registros ativos, eliminando fragmentação e calculando ganho de eficiência.

Casos de Teste

- Registros variáveis → melhor aproveitamento dos blocos.
- Registros fixos → maior fragmentação interna.
- Exclusão lógica → reaproveitamento eficiente de espaço.
- Reorganização → ganho de eficiência proporcional ao nível de fragmentação existente.

Conclusão

O trabalho atingiu todos os objetivos, implementando um sistema completo de armazenamento em blocos com suporte a registros fixos e variáveis.
As operações desenvolvidas permitiram analisar, na prática, os impactos da fragmentação e da reorganização, reforçando conceitos fundamentais de Algoritmos e Estruturas de Dados II.
