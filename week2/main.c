#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/**
 * @file ex2.c
 * @brief �? b�i: Get host's IP address by name
 * �?u v�o: domain name
 * �?u ra: IP address
 *
 * Example input: ./ex2 google.com
 * Example output: 142.250.66.46 (An IP Address in ASCII)
 *
 * N?i socket v�o th? IP d�
 */

#define INVALID_SOCKET -1

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        char *domain = argv[1];
        // char *domain = "vnexpress.net";
        struct hostent *presult = gethostbyname(domain); // presult: Pointer Result
        if (presult != NULL)
        {
            struct hostent host = *presult;
            int count = 0;
            int type = host.h_addrtype;
            struct in_addr inaddr;
            while (host.h_addr_list[count] != NULL)
            {
                char *addr = host.h_addr_list[count]; // 4 bytes d?a ch? IP
                memcpy(&inaddr, addr, sizeof(inaddr));
                // Chuy?n d?i addr -> ASCII cho d? d?c
                char *saddr = inet_ntoa(inaddr); // ntoa: Net to Address
                printf("%s\n", saddr);
                count += 1;
            }

            if (count > 0)
            {
                // T?o 1 socket m?i
                int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (s != INVALID_SOCKET)
                {
                    struct sockaddr_in addr;
                    addr.sin_family = AF_INET;
                    addr.sin_addr = inaddr;
                    addr.sin_port = htons(80); // htons: Host To Net Short
                    // K?t n?i
                    int error = connect(s, (struct sockaddr *)&addr, sizeof(addr));
                    if (error == 0) // N?u connect th�nh c�ng
                    {
                        char buffer[1024] = {0};
                        // G?i message b?t k? cho trang web
                        // G?i l?nh d�ng chu?n giao th?c HTTP (S? tr? v? l?i Moved Permanently nhung ksao)
                        char *hello = "GET / HTTP/1.1\r\nHost: nginx\r\n\r\n";
                        int sent = send(s, hello, strlen(hello), 0);
                        int received = recv(s, buffer, sizeof(buffer), 0);
                        printf("%d %s\n", received, buffer);
                    }
                }
            }
        }
    }
}
