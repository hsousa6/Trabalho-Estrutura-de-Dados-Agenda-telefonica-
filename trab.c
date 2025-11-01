#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Contato {
    char nome[50];
    char telefone[20];
    struct Contato *proximo;
} Contato;


int comparaIgnorandoCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower(*a) != tolower(*b))
            return (tolower(*a) - tolower(*b));
        a++;
        b++;
    }
    return (tolower(*a) - tolower(*b));
}


int telefoneValido(const char *telefone) {
    int len = strlen(telefone);
    if (len == 0) return 0; 
    if (telefone[0] == '-' || telefone[len - 1] == '-') return 0;

    for (int i = 0; i < len; i++) {
        if (!isdigit(telefone[i]) && telefone[i] != '-')
            return 0; 
    }
    return 1;
}


Contato* criarContato(char nome[], char telefone[]) {
    Contato *novo = (Contato*) malloc(sizeof(Contato));
    strcpy(novo->nome, nome);
    strcpy(novo->telefone, telefone);
    novo->proximo = NULL;
    return novo;
}


void salvarContatos(Contato *lista) {
    FILE *arquivo = fopen("agenda.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao salvar contatos!\n");
        return;
    }
    while (lista != NULL) {
        fprintf(arquivo, "%s;%s\n", lista->nome, lista->telefone);
        lista = lista->proximo;
    }
    fclose(arquivo);
}


void adicionarContato(Contato **lista) {
    char nome[50], telefone[20];
    printf("Digite o nome: ");
    scanf(" %[^\n]", nome);

    do {
        printf("Digite o telefone (somente números e '-'): ");
        scanf(" %[^\n]", telefone);
        if (!telefoneValido(telefone))
            printf("❌ Telefone inválido! Tente novamente.\n");
    } while (!telefoneValido(telefone));

    Contato *novo = criarContato(nome, telefone);

    
    if (*lista == NULL || comparaIgnorandoCase(novo->nome, (*lista)->nome) < 0) {
        novo->proximo = *lista;
        *lista = novo;
    } else {
        Contato *atual = *lista;
        while (atual->proximo != NULL && comparaIgnorandoCase(atual->proximo->nome, novo->nome) < 0)
            atual = atual->proximo;
        novo->proximo = atual->proximo;
        atual->proximo = novo;
    }

    salvarContatos(*lista);
    printf("✅ Contato adicionado e salvo com sucesso!\n");
}


void listarContatos(Contato *lista) {
    if (lista == NULL) {
        printf("A agenda está vazia.\n");
        return;
    }

    printf("\n--- Lista de Contatos ---\n");
    while (lista != NULL) {
        printf("Nome: %s | Telefone: %s\n", lista->nome, lista->telefone);
        lista = lista->proximo;
    }
    printf("--------------------------\n");
}


Contato* buscarContato(Contato *lista, char nome[]) {
    while (lista != NULL) {
        if (comparaIgnorandoCase(lista->nome, nome) == 0)
            return lista;
        lista = lista->proximo;
    }
    return NULL;
}


void buscarParcial(Contato *lista) {
    char termo[50];
    int encontrados = 0;
    printf("Digite parte do nome para buscar: ");
    scanf(" %[^\n]", termo);

    printf("\n--- Resultados da busca ---\n");
    while (lista != NULL) {
        char nomeMinusculo[50], termoMinusculo[50];
        strcpy(nomeMinusculo, lista->nome);
        strcpy(termoMinusculo, termo);
        for (int i = 0; nomeMinusculo[i]; i++) nomeMinusculo[i] = tolower(nomeMinusculo[i]);
        for (int i = 0; termoMinusculo[i]; i++) termoMinusculo[i] = tolower(termoMinusculo[i]);

        if (strstr(nomeMinusculo, termoMinusculo) != NULL) {
            printf("Nome: %s | Telefone: %s\n", lista->nome, lista->telefone);
            encontrados++;
        }
        lista = lista->proximo;
    }

    if (encontrados == 0)
        printf("Nenhum contato encontrado.\n");
    printf("---------------------------\n");
}


void atualizarContato(Contato *lista) {
    char nome[50], telefone[20];
    printf("Digite o nome do contato a atualizar: ");
    scanf(" %[^\n]", nome);

    Contato *contato = buscarContato(lista, nome);

    if (contato == NULL) {
        printf("Contato não encontrado!\n");
    } else {
        do {
            printf("Digite o novo telefone (somente números e '-'): ");
            scanf(" %[^\n]", telefone);
            if (!telefoneValido(telefone))
                printf("❌ Telefone inválido! Tente novamente.\n");
        } while (!telefoneValido(telefone));

        strcpy(contato->telefone, telefone);
        salvarContatos(lista);
        printf("✅ Telefone atualizado e salvo com sucesso!\n");
    }
}


void removerContato(Contato **lista) {
    char nome[50];
    printf("Digite o nome do contato a remover: ");
    scanf(" %[^\n]", nome);

    Contato *temp = *lista;
    Contato *anterior = NULL;

    while (temp != NULL && comparaIgnorandoCase(temp->nome, nome) != 0) {
        anterior = temp;
        temp = temp->proximo;
    }

    if (temp == NULL) {
        printf("Contato não encontrado!\n");
        return;
    }

    if (anterior == NULL)
        *lista = temp->proximo;
    else
        anterior->proximo = temp->proximo;

    free(temp);
    salvarContatos(*lista);
    printf("✅ Contato removido e arquivo atualizado!\n");
}


void carregarContatos(Contato **lista) {
    FILE *arquivo = fopen("agenda.txt", "r");
    if (arquivo == NULL)
        return;

    char linha[100], nome[50], telefone[20];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (sscanf(linha, "%49[^;];%19[^\n]", nome, telefone) == 2) {
            Contato *novo = criarContato(nome, telefone);
            if (*lista == NULL || comparaIgnorandoCase(novo->nome, (*lista)->nome) < 0) {
                novo->proximo = *lista;
                *lista = novo;
            } else {
                Contato *atual = *lista;
                while (atual->proximo != NULL && comparaIgnorandoCase(atual->proximo->nome, novo->nome) < 0)
                    atual = atual->proximo;
                novo->proximo = atual->proximo;
                atual->proximo = novo;
            }
        }
    }

    fclose(arquivo);
    printf("Contatos carregados de 'agenda.txt'.\n");
}


void liberarLista(Contato *lista) {
    Contato *temp;
    while (lista != NULL) {
        temp = lista;
        lista = lista->proximo;
        free(temp);
    }
}


int main() {
    Contato *lista = NULL;
    int opcao;

    carregarContatos(&lista);

    do {
        printf("\n===== AGENDA TELEFÔNICA =====\n");
        printf("1 - Adicionar contato\n");
        printf("2 - Listar contatos\n");
        printf("3 - Atualizar contato\n");
        printf("4 - Remover contato\n");
        printf("5 - Buscar contato (parcial)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarContato(&lista);
                break;
            case 2:
                listarContatos(lista);
                break;
            case 3:
                atualizarContato(lista);
                break;
            case 4:
                removerContato(&lista);
                break;
            case 5:
                buscarParcial(lista);
                break;
            case 0:
                salvarContatos(lista);
                liberarLista(lista);
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    return 0;
}
