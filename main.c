/*
 TP II – AEDS II
 Armazenamento em Blocos com Registros Fixos ou Variáveis
 Integra TP I + TP II
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= CONSTANTES ================= */
#define MAX_NOME 50
#define MAX_CURSO 30
#define MAX_FILIA 30
#define MAX_CPF 12

#define ARQ "alunos.dat"
#define ARQ_REORG "alunos_reorg.dat"

/* ================= ESTRUTURA ================= */
typedef struct {
    int matricula;
    char nome[MAX_NOME];
    char cpf[MAX_CPF];
    char curso[MAX_CURSO];
    char mae[MAX_FILIA];
    char pai[MAX_FILIA];
    int ano_ingresso;
    float coeficiente;
    int removido;   // 0 ativo | 1 removido
} Aluno;

/* ================= METADADOS DE BLOCO ================= */
typedef struct {
    int id;
    int usados;
} Bloco;

/* ================= VARIÁVEIS GLOBAIS ================= */
int TAM_BLOCO;
int TIPO_REG;          // 1 = fixo | 2 = variável
Bloco blocos[10000];
int total_blocos = 0;

/* ================= UTIL ================= */
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerLinha(char *buf, int tam) {
    fgets(buf, tam, stdin);
    buf[strcspn(buf, "\n")] = '\0';
}

/* ================= TAMANHO DO REGISTRO ================= */
int tamanhoRegistro(Aluno *a) {
    if (TIPO_REG == 1)
        return sizeof(Aluno);

    return sizeof(int)*3 + sizeof(float) +
           strlen(a->nome) + strlen(a->cpf) +
           strlen(a->curso) + strlen(a->mae) + strlen(a->pai);
}

/* ================= CONTROLE DE BLOCOS ================= */
void ocuparBloco(int bytes) {
    if (total_blocos == 0 || blocos[total_blocos - 1].usados + bytes > TAM_BLOCO) {
        blocos[total_blocos].id = total_blocos + 1;
        blocos[total_blocos].usados = bytes;
        total_blocos++;
    } else {
        blocos[total_blocos - 1].usados += bytes;
    }
}

/* ================= GERAR ALUNOS FICTÍCIOS ================= */
void gerarAlunoFicticio(Aluno *a, int i) {
    a->matricula = 1000 + i;
    sprintf(a->nome, "Aluno_%d", i);
    sprintf(a->cpf, "00000000%03d", i);
    sprintf(a->curso, "Curso_%d", i % 5);
    sprintf(a->mae, "Mae_%d", i);
    sprintf(a->pai, "Pai_%d", i);
    a->ano_ingresso = 2018 + (i % 5);
    a->coeficiente = 6.0 + (i % 4);
    a->removido = 0;
}

void gerarArquivoFicticio(int qtd) {
    FILE *f = fopen(ARQ, "wb");
    if (!f) return;

    Aluno a;
    total_blocos = 0;

    for (int i = 1; i <= qtd; i++) {
        gerarAlunoFicticio(&a, i);
        fwrite(&a, sizeof(Aluno), 1, f);
        ocuparBloco(tamanhoRegistro(&a));
    }
    fclose(f);
}

/* ================= INSERÇÃO ================= */
void inserir() {
    FILE *f = fopen(ARQ, "r+b");
    if (!f) f = fopen(ARQ, "w+b");

    Aluno a, temp;
    a.removido = 0;

    printf("Matricula: "); scanf("%d", &a.matricula); limpaBuffer();
    printf("Nome: "); lerLinha(a.nome, MAX_NOME);
    printf("CPF: "); lerLinha(a.cpf, MAX_CPF);
    printf("Curso: "); lerLinha(a.curso, MAX_CURSO);
    printf("Mae: "); lerLinha(a.mae, MAX_FILIA);
    printf("Pai: "); lerLinha(a.pai, MAX_FILIA);
    printf("Ano de ingresso: "); scanf("%d", &a.ano_ingresso);
    printf("Coeficiente: "); scanf("%f", &a.coeficiente);
    limpaBuffer();

    int tam = tamanhoRegistro(&a);

    rewind(f);
    long pos = 0;

    while (fread(&temp, sizeof(Aluno), 1, f)) {
        if (temp.removido) {
            fseek(f, pos, SEEK_SET);
            fwrite(&a, sizeof(Aluno), 1, f);
            fclose(f);
            printf("Registro inserido em espaco reaproveitado.\n");
            return;
        }
        pos += sizeof(Aluno);
    }

    fwrite(&a, sizeof(Aluno), 1, f);
    ocuparBloco(tam);
    fclose(f);
    printf("Registro inserido no final do arquivo.\n");
}

/* ================= EDIÇÃO ================= */
void editar() {
    int mat;
    printf("Matricula para editar: ");
    scanf("%d", &mat);
    limpaBuffer();

    FILE *f = fopen(ARQ, "r+b");
    if (!f) return;

    Aluno a;
    long pos;

    while ((pos = ftell(f)), fread(&a, sizeof(Aluno), 1, f)) {
        if (a.matricula == mat && !a.removido) {
            int tam_antigo = tamanhoRegistro(&a);

            printf("Novo nome: "); lerLinha(a.nome, MAX_NOME);
            printf("Novo curso: "); lerLinha(a.curso, MAX_CURSO);
            printf("Novo coeficiente: "); scanf("%f", &a.coeficiente);
            limpaBuffer();

            int tam_novo = tamanhoRegistro(&a);

            if (tam_novo <= tam_antigo) {
                fseek(f, pos, SEEK_SET);
                fwrite(&a, sizeof(Aluno), 1, f);
            } else {
                a.removido = 1;
                fseek(f, pos, SEEK_SET);
                fwrite(&a, sizeof(Aluno), 1, f);
                fclose(f);
                inserir();
                return;
            }

            fclose(f);
            printf("Registro editado com sucesso.\n");
            return;
        }
    }
    fclose(f);
    printf("Registro nao encontrado.\n");
}

/* ================= EXCLUSÃO ================= */
void excluir() {
    int mat;
    printf("Matricula para excluir: ");
    scanf("%d", &mat);
    limpaBuffer();

    FILE *f = fopen(ARQ, "r+b");
    if (!f) return;

    Aluno a;
    long pos;

    while ((pos = ftell(f)), fread(&a, sizeof(Aluno), 1, f)) {
        if (a.matricula == mat && !a.removido) {
            a.removido = 1;
            fseek(f, pos, SEEK_SET);
            fwrite(&a, sizeof(Aluno), 1, f);
            fclose(f);
            printf("Registro removido logicamente.\n");
            return;
        }
    }
    fclose(f);
    printf("Registro nao encontrado.\n");
}

/* ================= LISTAGEM ================= */
void listar() {
    FILE *f = fopen(ARQ, "rb");
    if (!f) return;

    Aluno a;
    int ativos = 0, removidos = 0;

    printf("\n===== REGISTROS =====\n");
    while (fread(&a, sizeof(Aluno), 1, f)) {
        if (!a.removido) {
            ativos++;
            printf("Mat:%d | Nome:%s | Curso:%s | Ano:%d | Coef:%.2f\n",
                   a.matricula, a.nome, a.curso, a.ano_ingresso, a.coeficiente);
        } else {
            removidos++;
        }
    }
    fclose(f);
    printf("Ativos: %d | Excluidos: %d\n", ativos, removidos);
}

/* ================= MAPA DE OCUPAÇÃO ================= */
void mapaBlocos() {
    printf("\n===== MAPA DE OCUPACAO =====\n");
    for (int i = 0; i < total_blocos; i++) {
        int livres = TAM_BLOCO - blocos[i].usados;
        printf("Bloco %d: %d usados | %d livres | %.1f%%\n",
               i + 1,
               blocos[i].usados,
               livres,
               (blocos[i].usados / (float)TAM_BLOCO) * 100.0);
    }
}

/* ================= REORGANIZAÇÃO ================= */
void reorganizar() {
    FILE *old = fopen(ARQ, "rb");
    FILE *novo = fopen(ARQ_REORG, "wb");
    if (!old || !novo) return;

    Aluno a;
    int total = 0, ativos = 0;

    while (fread(&a, sizeof(Aluno), 1, old)) {
        total++;
        if (!a.removido) {
            fwrite(&a, sizeof(Aluno), 1, novo);
            ativos++;
        }
    }

    fclose(old);
    fclose(novo);

    int blocosAntes = (total * sizeof(Aluno) + TAM_BLOCO - 1) / TAM_BLOCO;
    int blocosDepois = (ativos * sizeof(Aluno) + TAM_BLOCO - 1) / TAM_BLOCO;

    float ocupAntes = (float)(total * sizeof(Aluno)) / (blocosAntes * TAM_BLOCO) * 100;
    float ocupDepois = (float)(ativos * sizeof(Aluno)) / (blocosDepois * TAM_BLOCO) * 100;

    printf("\n===== RELATORIO DE REORGANIZACAO =====\n");
    printf("Antes:\nBlocos: %d\nOcupacao media: %.1f%%\nEficiência total: %.1f%%\n",
           blocosAntes, ocupAntes, ocupAntes);
    printf("Depois:\nBlocos: %d\nOcupacao media: %.1f%%\nEficiência total: %.1f%%\n",
           blocosDepois, ocupDepois, ocupDepois);
    printf("Ganho de eficiencia: %.1f%%\n", ocupDepois - ocupAntes);
    printf("Blocos liberados: %d\n", blocosAntes - blocosDepois);
    printf("=====================================\n");
}

/* ================= MENU ================= */
int main() {
    int op, gerar, qtd;

    printf("Deseja gerar registros ficticios?\n1-Sim | 0-Nao: ");
    scanf("%d", &gerar);

    if (gerar == 1) {
        printf("Quantidade de registros: ");
        scanf("%d", &qtd);
    }

    printf("Tamanho do bloco (bytes): ");
    scanf("%d", &TAM_BLOCO);

    printf("Tipo de registro (1-Fixo | 2-Variavel): ");
    scanf("%d", &TIPO_REG);
    limpaBuffer();

    if (gerar == 1)
        gerarArquivoFicticio(qtd);

    do {
        printf("\n=== TP II - ARMAZENAMENTO EM BLOCOS ===\n");
        printf("1. Inserir aluno\n2. Editar aluno\n3. Excluir aluno\n4. Listar registros\n5. Mapa de blocos\n6. Reorganizar arquivo\n0. Sair\nEscolha: ");
        scanf("%d", &op);
        limpaBuffer();

        switch (op) {
            case 1: inserir(); break;
            case 2: editar(); break;
            case 3: excluir(); break;
            case 4: listar(); break;
            case 5: mapaBlocos(); break;
            case 6: reorganizar(); break;
        }
    } while (op != 0);

    return 0;
}

